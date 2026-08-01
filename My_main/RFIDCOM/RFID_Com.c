#include "RFID_Com.h"
#include "cmsis_os2.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;

static uint8_t g_rfid_card_present = 0;
static uint8_t g_rfid_missing_count = 0;
static uint32_t g_rfid_check_tick = 0;

#define RFID_UID_LENGTH (4)
// #define RFID_CHECK_INTERVAL_MS     (100)
#define RFID_REMOVE_CONFIRM_COUNT  (3)

#define RFID_AUTHORIZED_CARD_COUNT \
    (sizeof(g_authorized_cards) / sizeof(g_authorized_cards[0]))

static const RFID_AuthorizedCard g_authorized_cards[] =
{
    { { 0xA2, 0xD2, 0x13, 0x07 }, "DongMin" },
    { { 0xFF, 0xFF, 0xFF, 0xFF }, "Admin"  },
    { { 0x91, 0xCD, 0x33, 0x07 }, "DongMin_2"  },
};

static uint8_t g_rfid_current_uid[RFID_UID_LENGTH] = {0};


static const RFID_AuthorizedCard* RFID_FindAuthorizedCard(const uint8_t* uid)
{
    if(uid == NULL)
        return NULL;

    for(uint32_t i = 0; i < RFID_AUTHORIZED_CARD_COUNT; i++)
    {
        if(memcmp(uid, g_authorized_cards[i].uid, RFID_UID_LENGTH) == 0)
            return &g_authorized_cards[i];
    }

    return NULL;
}

static void RFID_HandleDetectedCard(const uint8_t* uid)
{
    const RFID_AuthorizedCard* authorizedCard;

    if(uid == NULL)
        return;

    printf("Card ON UID: %02X %02X %02X %02X\r\n", uid[0], uid[1], uid[2], uid[3]);

    authorizedCard = RFID_FindAuthorizedCard(uid);

    if(authorizedCard != NULL)
    {
        printf("RFID Authorized: %s\r\n", authorizedCard->name);

        System_PostFlag(CONTROL_FLAG_RFID_AUTHORIZED);
    }
    else
    {
        printf("RFID Unknown UID: %02X %02X %02X %02X\r\n", uid[0], uid[1], uid[2], uid[3]);

        System_RequestAuthFailure();
        System_PostFlag(CONTROL_FLAG_RFID_UNKNOWN);
    }
}


// static void RFID_Success(const RFID_AuthorizedCard* card)
// {
//     printf("RFID Authorized: %s\r\n", card->name);

//     HAL_GPIO_WritePin(PC0_A5_RELAY_GPIO_Port, PC0_A5_RELAY_Pin, GPIO_PIN_SET);
// }

// static void RFID_Failed(const uint8_t* uid)
// {
//     printf("RFID Failed UID: %02X %02X %02X %02X\r\n", uid[0], uid[1], uid[2], uid[3]);

//     HAL_GPIO_WritePin(PC0_A5_RELAY_GPIO_Port, PC0_A5_RELAY_Pin, GPIO_PIN_RESET);
// }


void RFID_Process(void)
{
    uint8_t status;
    uint8_t tagType[2] = {0};
    uint8_t uid[5] = {0};

    if(!g_rfid_card_present)
    {
        status = RC522_Request(PICC_REQIDL, tagType);

        if(status != MI_OK)
            return;

        /*
         * 카드가 응답하면 UID를 읽는다.
         */
        status = RC522_Anticoll(uid);

        if(status != MI_OK)
            return;

        memcpy(g_rfid_current_uid, uid, RFID_UID_LENGTH);

        g_rfid_card_present = 1;
        g_rfid_missing_count = 0;

        RFID_HandleDetectedCard(uid);
        return;
    }

    /*
     * 카드가 이미 감지된 상태
     */
    status = RC522_Anticoll(uid);

    if(status == MI_OK)
    {
        g_rfid_missing_count = 0;

        if(memcmp(g_rfid_current_uid, uid, RFID_UID_LENGTH) != 0)
        {
            memcpy(g_rfid_current_uid, uid, RFID_UID_LENGTH);
            RFID_HandleDetectedCard(uid);
        }

        return;
    }

    g_rfid_missing_count++;

    if(g_rfid_missing_count < RFID_REMOVE_CONFIRM_COUNT)
        return;

    g_rfid_card_present = 0;
    g_rfid_missing_count = 0;

    memset(g_rfid_current_uid, 0, sizeof(g_rfid_current_uid));

    printf("Card Removed.\r\n");
}


