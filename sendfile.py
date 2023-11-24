#!/usr/bin/python3

import sys, datetime
from ftplib import FTP

def printf(string):
    print(datetime.datetime.strftime(datetime.datetime.now(), '%Y-%m-%d %H:%M:%S') + " : " + string)

if __name__ == '__main__':
    printf("FTP File Sender\n")
    try:
        if len(sys.argv)<5:
            print(f"Usage:\n  {sys.argv[0]} filename ftppath hostIP port")
            sys.exit(1)
        filename = sys.argv[1]
        path = sys.argv[2]
        host = sys.argv[3]
        port = int(sys.argv[4])

        ftp = FTP()
        printf(f"Connecting to {host}:{port}")
        ftp.connect(host, port)
        printf("Connected")

        printf("Opening " + filename)
        file = open(sys.argv[1], "rb")
        filename = "/{}".format(filename).replace("\\","/").split("/")[-1]
        printf("Success");

        printf("Moving to: ftp:/" + path)
        ftp.cwd(path)
        printf("Sending file")
        ftp.storbinary('STOR '+ filename, file)
        printf("Done")

        file.close()

        ftp.quit()
        printf("Disconnected")

    except Exception as e:
        printf("/!\ An error occured. /!\ ")
        print(e)
