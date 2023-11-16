#!/usr/bin/python3

import sys, datetime
from ftplib import FTP

# Usage:
# 	sendfile.py "filename" "ftppath" "hostIP" "port"

def printf(string):
	print(datetime.datetime.strftime(datetime.datetime.now(), '%Y-%m-%d %H:%M:%S') + " : " + string);

if __name__ == '__main__':
	print("");
	printf("FTP File Sender\n")
	try:
		if len(sys.argv)<5:
			print("Usage:\n  %s filename remotePath hostIP port" % sys.argv[0])
			sys.exit(1)
		filename = sys.argv[1]
		path = sys.argv[2]
		host = sys.argv[3]
		port = int(sys.argv[4])

		ftp = FTP()
		printf("Connecting to " + host + ":" + str(port));
		ftp.connect(host, port);
		printf("Connected");

		printf("Opening " + filename);
		file = open(sys.argv[1], "rb");
		filename = "/{}".format(filename).replace("\\","/").split("/")[-1]
		printf("Success");

		printf("Moving to: ftp:/" + path);
		ftp.cwd(path);
		printf("Sending file");
		ftp.storbinary('STOR '+ filename, file);
		printf("Done")

		file.close();

		ftp.quit();
		printf("Disconnected");

	except Exception as e:
		printf("/!\ An error occured. /!\ ");
		print(e);

