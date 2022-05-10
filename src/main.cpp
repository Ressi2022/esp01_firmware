/*
 * Copyright (C) 2015 Southern Storm Software, Pty Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
/*
This example explains basic AES128 implementation.
In Arduino serial monitor text appears to be non readable characters, but if you use any other serial terminals you can see the hex values
Example contributor: Aswin
*/
#include <Arduino.h>
#include <Crypto.h>
#include <AES.h>
#include <string.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>

const char ssid[] = "WifiReSsid";
const char pass[] = "E_RV{w4jz<F{";

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

void connect()
{
    Serial.print("checking wifi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }

    Serial.print("\nconnecting...");
    while (!client.connect("arduino", "public", "public"))
    {
        Serial.print(".");
        delay(1000);
    }

    Serial.println("\nconnected!");

    client.subscribe("/#");
    // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload)
{
    Serial.println("incoming: " + topic + " - " + payload);

    // Note: Do not use the client in the callback to publish, subscribe or
    // unsubscribe as it may cause deadlocks when other things arrive while
    // sending and receiving acknowledgments. Instead, change a global variable,
    // or push to a queue and handle it in the loop after calling `client.loop()`.
}

//key[16] cotain 16 byte key(128 bit) for encryption
byte key[16]= {'A', 'A', 'A', 'B', 'C', 'D', 'D', 'D','C', 'B', 'B', 'B', 'A', 'A', 'A', 'A'};
//plaintext[16] contain the text we need to encrypt
byte plaintext[16]="Hello World !!!";
//cypher[16] stores the encrypted text
byte cypher[16];
//decryptedtext[16] stores decrypted text after decryption
byte decryptedtext[16];
//creating an object of AES128 class
AES128 aes128;


void setup()
{
    Serial.begin(115200);

    WiFi.begin(ssid, pass);

    client.onMessage(messageReceived);
    client.begin("192.168.1.43", net);


    aes128.setKey(key,16);// Setting Key for AES

    Serial.print("Before Encryption:");
    for(unsigned int i=0; i<sizeof(plaintext); i++)
    {
        Serial.write(plaintext[i]);
    }

    aes128.encryptBlock(cypher,plaintext);//cypher->output block and plaintext->input block
    Serial.println();
    Serial.print("After Encryption:");
    for(unsigned int j=0; j<sizeof(cypher); j++)
    {
        Serial.write(cypher[j]);
    }

    aes128.decryptBlock(decryptedtext,cypher);

    Serial.println();
    Serial.print("After Dencryption:");
    for(unsigned int i=0; i<sizeof(decryptedtext); i++)
    {
        Serial.write(decryptedtext[i]);
    }
}

void loop()
{
    if (WiFi.status() != WL_CONNECTED){

    }else{
        if (!client.connected())
        {
            connect();
        }else{
            client.loop();
            // publish a message roughly every second.
            if (millis() - lastMillis > 1000)
            {
                lastMillis = millis();
                client.publish("/hello", "world");
            }
        }
    }
    delay(10);  // <- fixes some issues with WiFi stability
}
