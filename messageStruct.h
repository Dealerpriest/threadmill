struct SerialMessage {
  char startChar[4] = "<<<";
  int32_t touchValues[6];
  int32_t loadCellValues[4];
  char endChar[4] = ">>>";
};

const int serialMessageNrOfTouchValues =
    sizeof(SerialMessage::touchValues) / sizeof(int);
const int serialMessageNrOfLoadCellValues =
    sizeof(SerialMessage::loadCellValues) / sizeof(int);