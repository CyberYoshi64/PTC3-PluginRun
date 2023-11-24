#!/usr/bin/python3

import os, glob, shutil

USE_NINTYS_STUFF = False # I'm a good boy... I would hope.

prj = glob.glob("*_*/textures")
img = glob.glob("common/textures/*_tex.*")

try: shutil.rmtree("temp")
except: pass
os.mkdir("temp")

imagePropN = []
imagePropM = []

for i in prj:
	shutil.rmtree(i)
	os.mkdir(i)

try:
	with open("common/textures/prop.txt","r",encoding="utf-8") as f:
		fs = f.seek(0,2); f.seek(0,0)
		while f.tell()<fs:
			s = f.readline().strip().split("\t")
			if len(s)<2 or len(s[0])<3: continue
			imagePropN.append(s[0])
			imagePropM.append(s[1:])
			if len(imagePropM[-1])>1: imagePropM[-1][1] = int(imagePropM[-1][1],0)
			if len(imagePropM[-1])>2: imagePropM[-1][2] = int(imagePropM[-1][2],0)
except: pass

for i in img:
	name = i[i.rfind(os.sep)+1:i.rfind("_tex.")]
	index = len(imagePropM)
	try: index = imagePropN.index(name)
	except:
		imagePropN.append(name)
		imagePropM.append(["ETC1_A4",0,0,"default"])

	print()
	print(name, end="", flush=True)
	if USE_NINTYS_STUFF:
		os.system("magick {} temp/temp.tga".format(i))
		print(end=".",flush=True)
		os.system("./"*(os.name!="nt")+"NW4C_TextureConverter.exe temp/temp.tga -o=temp/out.tga -f={} -ap".format(imagePropM[index][0]))
	else:
		os.system("python nw4c-textureConverter.py -o temp/out.tga -f {} {} {} {}".format(
			imagePropM[index][0],
			"-r {},{}".format(imagePropM[index][1],imagePropM[index][2])*(imagePropM[index][1]>0),
			"-ee {}".format(imagePropM[index][3])*(imagePropM[index][3]!="default"),
			i
		))
	print(end=".",flush=True)
	for j in prj:
		shutil.copyfile("temp/out.tga","{}/{}.tga".format(j, name))
		print(end=".",flush=True)

print()
with open("images/prop.txt","w",encoding="utf-8") as f:
	f.truncate(0)
	for i in range(len(imagePropM)):
		f.write(imagePropN[i])
		for j in imagePropM[i]:
			f.write("\t{}".format(j))
		f.write("\n")