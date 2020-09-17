#include <Mouse.h>

const byte maxChars = 64;
char xChars[maxChars];
char yChars[maxChars];

bool nxtData = false;
bool newData = false;

void setup() 
{
  Serial.begin(9600);
  Mouse.begin();
}

void loop() {
  receiveMessage();
  processMessage();
}

void receiveMessage() 
{
    byte ndx = 0;
    byte ndy = 0;
    char endMarker = '\n';
    char rc;
   
    while (Serial.available() > 0 && newData == false) 
    {
        rc = Serial.read();
        if (rc != endMarker) 
        {
            if(rc != ':')
            {
              if(nxtData == false)
              {
                xChars[ndx] = rc;
                ndx++;
              }
              else
              {
                yChars[ndy] = rc;
                ndy++;
              }

              // Overflow protection
              if (ndx >= maxChars)
              {
                  ndx = maxChars - 1;
              }
              if (ndy >= maxChars)
              {
                  ndy = maxChars - 1;
              }
            }
            else {
              nxtData = true; 
            }
        }
        else 
        {
            xChars[ndx] = '\0';
            yChars[ndy] = '\0';
            newData = true;
            nxtData = false;
        }
    }
}

void processMessage()
{
  if(newData == true)
  {
    Mouse.move(atoi(xChars), atoi(yChars), 0);
    newData = false; // reset
  }

}
