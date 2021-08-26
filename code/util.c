
inline void Swap(void * restrict A, void * restrict B, size Size)
{
    if (Size == 0 || A == B) 
        return;
    
    u8 *A8 = (u8 *)A;
    u8 *B8 = (u8 *)B;
    while (Size--) { u8 T8 = *A8; *A8++ = *B8; *B8++ = T8; }
}

typedef b32 ez_compare_func(const void*, const void*);

void Sort__Insertion(void *Array, size Count, const size Size, ez_compare_func *Compare)
{
#define PTR_ADD(P,A) (void*)((u8*)(P)+(A))
#define PTR_SUB(P,A) (void*)((ptr)(P)-(A))
    void *End = PTR_ADD(Array, (Count-1) * Size);
    void *Cur = Array;
    while (Cur < End)
    {
        void *B = Cur;
        Cur = PTR_ADD(Cur, Size);
        void *A = Cur;
        while (Compare(B, A))
        {
            Swap(B, A, Size);
            if (B == Array) break;
            A = B;
            B = PTR_SUB(B, Size);
        }
    }
#undef PTR_ADD
#undef PTR_SUB
}

#define Sort_Insertion(ARRAY, COUNT, ...) _Generic((), \
    default: Sort__Insertion((ARRAY), (COUNT), sizeof((ARRAY)[0]), __VA_ARGS__) \
)
