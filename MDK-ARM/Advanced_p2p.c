// Includes
#ifdef Advanced
#include "Advanced_p2p.h"
#include "main.h"
#include "p2p_stm.h"
#include "p2p_server_app.h"
#include "common_blesvc.h"
#include "ble_types.h"
//Private TypeDefs



//Private Variables





//Private Faunction Prototypes

tBleStatus P2PS_STM_App_Update_Int16_Pro(void);





//Function Definitions

COPY_P2P_READ_UUID(uuid16.Char_UUID_128)
aci_gatt_add_char(CustomContext.CustomMy_HrsHdle,
                  UUID_TYPE_16, &uuid,
                  SizeHrs_Sl,
                  CHAR_PROP_READ,
                  ATTR_PERMISSION_NONE,
                  GATT_DONT_NOTIFY_EVENTS,
                  0x10,
                  CHAR_VALUE_LEN_CONSTANT,
                  &(CustomContext.CustomHrs_SlHdle));

tBleStatus P2PS_STM_App_Update_Int16(uint16_t UUID, uint16_t *pPayload, uint8_t num_words)
{

  uint16_t byte_reversed[num_words];

  for (uint8_t i = 0; i < num_words; i++)
    {
      byte_reversed[i] = (pPayload[i] & 0xFF00) >> 8 | (pPayload[i] & 0x00FF) << 8;
    }

  tBleStatus result = BLE_STATUS_INVALID_PARAMS;
  switch(UUID)
    {
    case P2P_NOTIFY_CHAR_UUID:

      result = aci_gatt_update_char_value(aPeerToPeerContext.PeerToPeerSvcHdle,
                                          aPeerToPeerContext.P2PNotifyServerToClientCharHdle,
                                          0, /* charValOffset */
                                          2*num_words, /* charValueLen */
                                          (uint8_t *)  byte_reversed);

      break;

    default:
      break;
    }

  return result;
}

tBleStatus P2PS_STM_App_Update_Int8(uint16_t UUID, uint8_t *pPayload, uint8_t num_bytes)
{
  tBleStatus result = BLE_STATUS_INVALID_PARAMS;
  switch(UUID)
    {
    case P2P_NOTIFY_CHAR_UUID:

      result = aci_gatt_update_char_value(aPeerToPeerContext.PeerToPeerSvcHdle,
                                          aPeerToPeerContext.P2PNotifyServerToClientCharHdle,
                                          0, /* charValOffset */
                                          num_bytes, /* charValueLen */
                                          (uint8_t *)  pPayload);

      break;

    default:
      break;
    }

  return result;
}
#endif