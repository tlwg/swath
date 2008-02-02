
bool isSpace(char ch)
{
	if ((ch==0x20) || ((ch<=0x0D)&&(ch >=0x09)))
		return true;
	else
		return false;
}

bool isHex(char ch)
{	//ascii 48 ='0' 57='9' and 97 = 'a' 102='f'
	if (((ch>=48)&&(ch<=57)) || ((ch>=97)&&(ch<=102)))
		return true;
	else
		return false;
}

bool isPunc(char ch)
{

//	if (  ((ch>=33)&&(ch<=47)) || ((ch>=58)&&(ch<=63))
//		      || ((ch>=91)&&(ch<=96))  )
	if ((ch=='\'')||(ch=='"')||(ch==96)||(ch==39)||(ch=='.') )
		return true;
	else 
		return false;
}
