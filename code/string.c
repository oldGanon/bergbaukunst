
typedef struct string
{
    const char *Data;
    u64 Length;
} string;

#define STR(s) {s,sizeof(s)-1}
#define STRING(s) (string)STR(s)

string String_FromCString(const char *CString)
{
    u64 Length = 0;
    while (CString[Length] != 0)
        ++Length;

    return (string) {
        .Data = CString,
        .Length = Length,
    };
}

bool String_Equal(string A, string B)
{
    if (A.Length != B.Length) return false;
    for (u64 i = 0; i < A.Length; ++i)
        if (A.Data[i] != B.Data[i])
            return false;
    return true;
}

string String_Trim(string String)
{
    while (String.Length)
    {
        if (String.Data[0] != ' ') break;
        ++String.Data;
        --String.Length;
    }
    return String;
}

string String_ExtractToken(string *String)
{
    *String = String_Trim(*String);

    string Token = {
        .Length = 0,
        .Data = String->Data,
    };

    while (String->Length)
    {
        if (Token.Data[Token.Length] == ' ') break;
        ++Token.Length;
        ++String->Data;
        --String->Length;
    }

    *String = String_Trim(*String);    
    return Token;
}
