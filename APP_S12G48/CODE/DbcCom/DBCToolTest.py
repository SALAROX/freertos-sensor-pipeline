import cantools
from pprint import pprint
db = cantools.database.load_file('FM29_NFC_Readers_CAN_v1.0.dbc')
example_message = db.get_message_by_name('NFCExt_0x101_ExtendedStsFrame')
pprint(example_message.signals)
#generate_c_source "FM29_NFC_Readers_CAN_v1.0.dbc"
