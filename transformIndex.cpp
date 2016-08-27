int transformIndex(int originIndex, int destOffset, int destArrayLength, int direction) {
  return (destOffset + originIndex * direction) % destArrayLength;
}
