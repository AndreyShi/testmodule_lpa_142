import sys

with open(sys.argv[2],'wb') as result_file:
  result_file.write(b'const char %s[] = {' % sys.argv[3].encode('utf-8'))
  for b in open(sys.argv[1], 'rb').read():
    result_file.write(b'0x%02X,' % b)
  result_file.write(b'};')