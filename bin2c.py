import sys
# sys.argv[0] - name this file
# sys.argv[1] - input bin file (*.bin) образ скаченный 
# sys.argv[2] - output file (*.h)
# sys.argv[3] - variable name
with open(sys.argv[2],'wb') as result_file:
  result_file.write(b'#include <stdint.h>\r\n')
  result_file.write(b'#ifdef VER124\r\n')
  result_file.write(b'__attribute__(( section(".firmwareSection") ))\r\n')
  result_file.write(b'static const uint8_t %s[] = {\r\n' % sys.argv[3].encode('utf-8'))
  pe = 0
  sz = 0
  for b in open(sys.argv[1], 'rb').read():
    result_file.write(b'0x%02X,' % b)
    pe = pe + 1
    if pe == 16:
      result_file.write(b'\r\n')
      pe = 0
    sz = sz + 1 # подсчитываем размер файла bin
  result_file.write(b'};\r\n')
  result_file.write(b'#endif\r\n')
  print('размер ',sys.argv[1],': ', sz,sep='')