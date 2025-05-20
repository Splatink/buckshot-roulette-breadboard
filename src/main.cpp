#include <Arduino.h>

#define SER PB0
#define OE PA7
#define RCLK PA6
#define SRCLK PA5
#define SRCLR PA4
#define BUTTON PA0
#define BLANK 0
#define LETHAL 1
#define DEALER_LED PB11
#define PLAYER_LED PB15 

unsigned int totalHealth;
unsigned int roundCount = 1;
bool turn;
unsigned int numberOfBlanks;
unsigned int numberOfLethals;
unsigned int totalShellCount;
unsigned int dealerHealth;
unsigned int playerHealth;
bool inRoundCount;
bool roundCleared = true;
int shellCount;
char blankshells[4] = {PB12, PB13, PB14, PA8};
char liveshells[4] = {PB9, PA9 ,PB5, PB6};
char shellOrder[8] = {5, 5, 5, 5, 5, 5, 5, 5};

void updatePlayersHealth()
{
  byte playerHealthHalfBit;
  byte dealerHealthHalfBit;
  switch(playerHealth)
  {
    case 1:
      playerHealthHalfBit = 0x8;
      break;
    case 2:
      playerHealthHalfBit = 0xC;
      break;
    case 3:
      playerHealthHalfBit = 0xE;
      break;
    case 4:
      playerHealthHalfBit = 0xF;
      break;
  }
  switch(dealerHealth)
  {
    case 1:
      dealerHealthHalfBit = 0x8;
      break;
    case 2:
      dealerHealthHalfBit = 0xC;
      break;
    case 3:
      dealerHealthHalfBit = 0xE;
      break;
    case 4:
      dealerHealthHalfBit = 0xF;
      break;
  }
  shiftOut(SER, SRCLK, LSBFIRST, (dealerHealthHalfBit << 4) | playerHealthHalfBit);
  digitalWrite(RCLK, 1);
  digitalWrite(RCLK, 0);
}

void drawPlayersHealth()
{
  totalHealth = random(2,5);
  dealerHealth = totalHealth;
  playerHealth = totalHealth;
}

void tellRoundCount()
{
  for (int i = 0; i < 4; i ++)
  {
    switch(roundCount)
    {
      case 1:
        shiftOut(SER, SRCLK, LSBFIRST, 0x88);
        break;
      case 2:
        shiftOut(SER, SRCLK, LSBFIRST, 0xCC);
        break;
      case 3:
        shiftOut(SER, SRCLK, LSBFIRST, 0xEE);
        break; 
    }
    digitalWrite(RCLK, HIGH);
    digitalWrite(RCLK, LOW);
    delay(500);
    shiftOut(SER, SRCLK, LSBFIRST, 0x00);
    digitalWrite(RCLK, HIGH);
    digitalWrite(RCLK, LOW);
    delay(500);
  }
}

void showBullets()
{
  for(int i = 0; i < numberOfBlanks; i++)
  {
    digitalWrite(blankshells[i], 1);
  }
  for(int j = 0; j < numberOfLethals; j++)
  {
    digitalWrite(liveshells[j], 1);
  }
  delay(3000);
  for(int x = 0; x < 4; x++)
  {
    digitalWrite(liveshells[x], 0);
    digitalWrite(blankshells[x], 0);
  }
}

void updateTurnLights()
{
  if (turn == false)
  {
    digitalWrite(PLAYER_LED, 1);
    digitalWrite(DEALER_LED, 0);
  }
  else
  {
    digitalWrite(DEALER_LED, 1);
    digitalWrite(PLAYER_LED, 0);
  }
}

void insertBullets()
{
  numberOfBlanks = 0;
  numberOfLethals = 0;
  totalShellCount = random(2, 9);
  for (int i = 0; i < totalShellCount; i++)
  {
    shellOrder[i] = random(2);
  }
  for (int a = 0; a < totalShellCount; a++)
  {
    bool shellType = shellOrder[a];
    if (shellType == BLANK)
    {
      numberOfBlanks++;
    }
    else
    {
      numberOfLethals++;
    }
  }
  shellCount = 0;
}

void blinkYourself(bool state)
{
  if (state == false)
  {
    for (int i = 0; i < 3; i++)
    {
      digitalWrite(PLAYER_LED, 1);
      delay(50);
      digitalWrite(PLAYER_LED, 0);
      delay(50);
    }
  }
  else
  {
    for (int i = 0; i < 3; i++)
    {
      digitalWrite(DEALER_LED, 1);
      delay(50);
      digitalWrite(DEALER_LED, 0);
      delay(50);
    }
  }
}

void fireBlank()
{
  for(int i = 0; i < 4; i++)
  {
    digitalWrite(blankshells[i], 1);
  }
  delay(200);
  for(int i = 0; i < 4; i++)
  {
    digitalWrite(blankshells[i], 0);
  }
}

void fireLive()
{
  for(int i = 0; i < 4; i++)
  {
    digitalWrite(liveshells[i], 1);
  }
  delay(200);
  for(int i = 0; i < 4; i++)
  {
    digitalWrite(liveshells[i], 0);
  }
}

void roundPlay()
{
  if (turn == false)
  {
    while (digitalRead(BUTTON) != 1)
    {
    }
    delay(1000);
    if (digitalRead(BUTTON) != 1) //player shoots dealer
    {
      if (shellOrder[shellCount] == BLANK)
      {
        shellCount++;
        numberOfBlanks--;
        fireBlank();
        turn = !turn;
      }
      else
      {
        shellCount++;
        numberOfLethals--;
        dealerHealth--;
        fireLive();
        turn = !turn;
      }
    }
    else
    {
      blinkYourself(false);
      if(shellOrder[shellCount] == BLANK) //player shoots himself
      {
        shellCount++;
        numberOfBlanks--;
        fireBlank();
      }
      else
      {
        shellCount++;
        numberOfLethals--;
        playerHealth--;
        fireLive();
        turn = !turn;
      }
    }
  }
  else
  {
    if (numberOfBlanks > numberOfLethals) //dealer shoots himself
    {
      blinkYourself(true);
      delay(1000);
      if (shellOrder[shellCount] == BLANK)
      {
        shellCount++;
        numberOfBlanks--;
        fireBlank();
      }
      else
      {
        shellCount++;
        numberOfLethals--;
        fireLive();
        dealerHealth--;
        turn = !turn;
      }
    }
    else if (numberOfBlanks == numberOfLethals) //random draw
    {
      unsigned int freedomOfChoise = random(2);
      if (freedomOfChoise == 0) // dealer randomly shoots player
      {
        if(shellOrder[shellCount] == BLANK)
        {
          shellCount++;
          numberOfBlanks--;
          fireBlank();
          turn = !turn;
        }
        else
        {
          shellCount++;
          numberOfLethals--;
          fireLive();
          playerHealth--;
          turn = !turn;
        }
      }
      else //dealer randomly shoots himself
      {  
        blinkYourself(true);
        delay(1000);
        if (shellOrder[shellCount] == BLANK)
        {
          shellCount++;
          numberOfBlanks--;
          fireBlank();
        }
        else
        {
          shellCount++;
          numberOfLethals--;
          fireLive();
          dealerHealth--;
          turn = !turn;
        }
      }
    }
    else //dealer shoots player
    {
      if(shellOrder[shellCount] == BLANK)
      {
        shellCount++;
        numberOfBlanks--;
        fireBlank();
        turn = !turn;
      }
      else
      {
        shellCount++;
        numberOfLethals--;
        fireLive();
        playerHealth--;
        turn = !turn;
      }
    }
  }
  updatePlayersHealth();
  updateTurnLights();
}

void checkHealth()
{
  if (playerHealth == 0)
  {
    roundCount = 1;
    roundCleared = true;
  }
  if (dealerHealth == 0)
  {
    roundCount++;
    roundCleared = true;
  }
  if ((numberOfBlanks + numberOfLethals == 0))
  {
    inRoundCount = true;
  }
}

void setup()
{
  pinMode(SER, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(SRCLK, OUTPUT);
  pinMode(SRCLR, OUTPUT);
  pinMode(DEALER_LED, OUTPUT);
  pinMode(PLAYER_LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLDOWN);
  pinMode(PB2, OUTPUT);

  for (int i = 0; i < 4; i++)
  {
    pinMode(blankshells[i], OUTPUT);
    pinMode(liveshells[i], OUTPUT);
  }

  digitalWrite(OE, LOW);
  digitalWrite(SRCLR, HIGH);

  randomSeed(analogRead(PB1));

}

void loop()
{
  if (roundCleared == true)
  {
    digitalWrite(PLAYER_LED, 0);
    digitalWrite(DEALER_LED, 0);
    turn = false;
    tellRoundCount();
    drawPlayersHealth();
    updatePlayersHealth();
    do
    {
      insertBullets();
    } while (numberOfBlanks > (totalShellCount / 2) || numberOfLethals > (totalShellCount / 2)); 
    showBullets();
    updateTurnLights();
    inRoundCount = false;
    roundCleared = false;
  }
  if (inRoundCount == true)
  {
    digitalWrite(PLAYER_LED, 0);
    digitalWrite(DEALER_LED, 0);
    do
    {
      insertBullets();
    } while (numberOfBlanks > (totalShellCount / 2) || numberOfLethals > (totalShellCount / 2)); 
    showBullets();
    turn = false;
    updateTurnLights();
    inRoundCount = false;
  }
  
  roundPlay();
  checkHealth();
  delay(3000);
  
  if(roundCount > 4)
  {
    digitalWrite(PB2, HIGH);
    while (1){}
  }
}