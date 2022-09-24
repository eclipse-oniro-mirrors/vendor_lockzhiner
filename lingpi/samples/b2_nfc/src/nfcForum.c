/*
 * Copyright (c) 2022 FuZhou Lockzhiner Electronic Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "nfcForum.h"
#include <string.h>

static void rtdHeader(uint8_t type, NDEFRecordStr *ndefRecord, uint8_t *I2CMsg)
{
    ndefRecord->header |= 1;
    ndefRecord->header |= BIT_SR;
    I2CMsg[0] = ndefRecord->header;
    
    ndefRecord->typeLength = 1;
    I2CMsg[1] = ndefRecord->typeLength;
    
    ndefRecord->type.typeCode = type;
    I2CMsg[3] = ndefRecord->type.typeCode;
}


uint8_t composeRtdText(const NDEFDataStr *ndef, NDEFRecordStr *ndefRecord, uint8_t *I2CMsg)
{
    uint8_t retLen;
    uint32_t offset_text = 4;
    uint8_t sizeof_header = 3;
    uint8_t sizeof_str = 3;
    uint8_t sizeof_type = 1;
    uint8_t offset_i2cmsg = 2;
    
    rtdHeader(RTD_TEXT, ndefRecord, I2CMsg);
    
    uint8_t payLoadLen = addRtdText(&ndefRecord->type.typePayload.text);
    memcpy(&I2CMsg[offset_text], &ndefRecord->type.typePayload.text, payLoadLen);
    
    ndefRecord->payloadLength = ndef->rtdPayloadlength + payLoadLen; // added the typePayload
    I2CMsg[offset_i2cmsg] = ndefRecord->payloadLength;
    
    retLen = sizeof_header + sizeof_str + sizeof_type;

    return retLen;
}


uint8_t composeRtdUri(const NDEFDataStr *ndef, NDEFRecordStr *ndefRecord, uint8_t *I2CMsg)
{
    uint32_t offset_text = 4;
    uint32_t offset_length = 2;
    uint8_t length = 5;
    
    rtdHeader(RTD_URI, ndefRecord, I2CMsg);
    
    uint8_t payLoadLen = addRtdUriRecord(ndef, &ndefRecord->type.typePayload.uri);
    memcpy(&I2CMsg[offset_text], &ndefRecord->type.typePayload.uri, payLoadLen);
    
    ndefRecord->payloadLength = ndef->rtdPayloadlength + payLoadLen; // added the typePayload
    I2CMsg[offset_length] = ndefRecord->payloadLength;
    
    return length;
}

void composeNDEFMBME(bool isFirstRecord, bool isLastRecord, NDEFRecordStr *ndefRecord)
{
    if (isFirstRecord) {
        ndefRecord->header |= BIT_MB;
    } else {
        ndefRecord->header &= ~MASK_MB;
    }
    
    if (isLastRecord) {
        ndefRecord->header |= BIT_ME;
    } else {
        ndefRecord->header &= ~MASK_ME;
    }
}
