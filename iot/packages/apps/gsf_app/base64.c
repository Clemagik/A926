/***************************************************************************************
 *
 *  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
 *
 *  By downloading, copying, installing or using the software you agree to this license.
 *  If you do not agree to this license, do not download, install, 
 *  copy or use the software.
 *
 *  Copyright (C) 2010-2014, ccr Corporation, all rights reserved.
 *
 *  Redistribution and use in binary forms, with or without modification, are permitted.
 *
 *  Unless required by applicable law or agreed to in writing, software distributed 
 *  under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 *  CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
 *  language governing permissions and limitations under the License.
 *
****************************************************************************************/

const char *BASE64_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void _base64_encode_triple(unsigned char triple[3], char result[4])
{
    int tripleValue, i;

    tripleValue = triple[0];
    tripleValue *= 256;
    tripleValue += triple[1];
    tripleValue *= 256;
    tripleValue += triple[2];

    for (i=0; i<4; i++)
    {
		result[3-i] = BASE64_CHARS[tripleValue%64];
		tripleValue /= 64;
    }
}

/**
 * encode an array of bytes using Base64 (RFC 3548)
 *
 * @param source the source buffer
 * @param sourcelen the length of the source buffer
 * @param target the target buffer
 * @param targetlen the length of the target buffer
 * @return 1 on success, 0 otherwise
 */
int gsf_base64_encode(unsigned char *source, unsigned int sourcelen, char *target, unsigned int targetlen)
{
  printf("#######121###########\n");
    /* check if the result will fit in the target buffer */
    if ((sourcelen+2)/3*4 > targetlen-1)
		return 0;
    printf("#######12###########\n");
    /* encode all full triples */
    while (sourcelen >= 3)
    {
      _base64_encode_triple(source, target);
      sourcelen -= 3;
      source += 3;
      target += 4;
    }
    printf("#######13###########\n");
    /* encode the last one or two characters */
    if (sourcelen > 0)
    {
		unsigned char temp[3];
		memset(temp, 0, sizeof(temp));
		memcpy(temp, source, sourcelen);
		_base64_encode_triple(temp, target);
		target[3] = '=';
		if (sourcelen == 1)
		    target[2] = '=';

		target += 4;
    }
    printf("#######14###########\n");
    /* terminate the string */
    target[0] = 0;

    return 1;
}


/**
 * encode an array of bytes using Base64 (RFC 3548)
 *
 * @param source the source buffer
 * @param sourcelen the length of the source buffer
 * @param target the target buffer
 * @param targetlen the length of the target buffer
 * @return 1 on success, 0 otherwise
 */
int base64_encode_dst(unsigned char *source, unsigned int sourcelen, char *target, unsigned int targetlen)
{
    /* check if the result will fit in the target buffer */
    if ((sourcelen+2)/3*4 > targetlen-1)
		return 0;

    /* encode all full triples */
    while (sourcelen >= 3)
    {
		_base64_encode_triple(source, target);
		sourcelen -= 3;
		source += 3;
		target += 4;
    }

    /* encode the last one or two characters */
    if (sourcelen > 0)
    {
		unsigned char temp[3];
		memset(temp, 0, sizeof(temp));
		memcpy(temp, source, sourcelen);
		_base64_encode_triple(temp, target);
		target[3] = '=';
		if (sourcelen == 1)
		    target[2] = '=';

		target += 4;
    }

    /* terminate the string */
    target[0] = 0;

    return target;
}



