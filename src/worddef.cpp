
bool isSpace(char ch)
{
	return (ch==' ') || ('\t'<=ch && ch<='\r');
}

bool isHex(char ch)
{
	return ('0'<=ch && ch<='9') || ('a'<=ch && ch<='f');
}

bool isPunc(char ch)
{
//	if (  ((ch>=33)&&(ch<=47)) || ((ch>=58)&&(ch<=63))
//		      || ((ch>=91)&&(ch<=96))  )
	return ch=='\'' || ch=='"' || ch=='`' || ch=='.';
}
