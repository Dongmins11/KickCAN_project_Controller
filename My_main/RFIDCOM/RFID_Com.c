#include "RFID_Com.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;

static uint8_t g_rfid_card_present = 0;
static uint8_t g_rfid_missing_count = 0;
static uint32_t g_rfid_check_tick = 0;

#define RFID_UID_LENGTH (4)
#define RFID_CHECK_INTERVAL_MS     (100)
#define RFID_REMOVE_CONFIRM_COUNT  (3)

#define RFID_AUTHORIZED_CARD_COUNT \
    (sizeof(g_authorized_cards) / sizeof(g_authorized_cards[0]))

static const RFID_AuthorizedCard g_authorized_cards[] =
{
    { { 0xA2, 0xD2, 0x13, 0x07 }, "DongMin" },
    { { 0xFF, 0xFF, 0xFF, 0xFF }, "Admin"  }
};


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
    uint8_t tagType[2] = {0, };
    uint8_t uid[5] = {0, };

    if(HAL_GetTick() - g_rfid_check_tick < RFID_CHECK_INTERVAL_MS)
        return;

    g_rfid_check_tick = HAL_GetTick();

    if(RC522_Request(PICC_REQIDL, tagType) == MI_OK)
    {
        g_rfid_missing_count = 0;

        if(g_rfid_card_present)
            return;

        if(RC522_Anticoll(uid) != MI_OK)
            return;

        g_rfid_card_present = 1;

        printf("Card oN UID: %02X %02X %02X %02X\r\n", uid[0], uid[1], uid[2], uid[3] );

        const RFID_AuthorizedCard* authorizedCard = RFID_FindAuthorizedCard(uid);

        if(card != NULL)
            System_PostFlag(CONTROL_FLAG_RFID_AUTHORIZED);
        else
            System_PostFlag(CONTROL_FLAG_RFID_UNKNOWN);
    }
    else
    {
        if(!g_rfid_card_present)
            return;

        g_rfid_missing_count++;

        if(g_rfid_missing_count < RFID_REMOVE_CONFIRM_COUNT)
            return;

        g_rfid_card_present = 0;
        g_rfid_missing_count = 0;

        printf("Card Removed.\r\n");
    }
}
