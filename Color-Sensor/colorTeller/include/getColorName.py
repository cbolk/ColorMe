def getColorName(strColor):
	colorName = strColor[0]
	for c in strColor[1:]:
		if c.isupper():
			colorName += " "
		colorName += c
	return colorName

f = open("colori.csv")
colreader = csv.reader(f)
dfnew = []
for row in colreader:
	if row[1].find(" ") > -1:
		row[1] = getColorName(row[1])
	dfnew.append([row[0],row[-1]])
f.close()
f = open("colorimin.csv")