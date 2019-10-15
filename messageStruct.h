struct SerialMessage {
  char startChar = '<';
  int touchValues[4];
  int loadCellValues[4];
  char endChar = '>';
};

const int serialMessageNrOfTouchValues = sizeof(SerialMessage::touchValues)/sizeof(int);
const int serialMessageNrOfLoadCellValues = sizeof(SerialMessage::loadCellValues)/sizeof(int);