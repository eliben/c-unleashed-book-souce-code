void WriteCharacterSet(char *filename)
{
  unsigned char ch;
  FILE *fp = fopen(filename, "wb");
  for(ch = 0; ch < 256; ch++)
  {
    fwrite(&ch, 1, 1, fp);
  }
  fclose(fp);
}
