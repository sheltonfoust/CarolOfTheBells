#define NUM_NOTES 34 * 6
#define R 0
#define C  1 << 0
#define D  1 << 1
#define E  1 << 2
#define F  1 << 3
#define G  1 << 4
#define A  1 << 5
#define B  1 << 6
#define HC 1 << 7



int main(void)
{

    int simpleDemo[NUM_NOTES] =
    {
        C,  R,      D,  E,      F,  G,
        A,  B,      HC, R,      R,  R,
        C + E + G, R,   R,  R,      R,  R
    };


    // Missing LowB, LowA, LowE, LowF#, Low G#
    // Not using B or HighC
    // Uses 11 bells in total
    int carolInMidC[NUM_NOTES] =
    {
        C,  R,      LB, C,      LA, R, // 12 measures of main theme
        C,  R,      LB, C,      LA, R,
        C,  R,      LB, C,      LA, R,
        C,  R,      LB, C,      LA, R,

        C,  R,      LB, C,      LA, R,
        C,  R,      LB, C,      LA, R,
        C,  R,      LB, C,      LA, R,
        C,  R,      LB, C,      LA, R,

        C,  R,      LB, C,      LA, R,
        C,  R,      LB, C,      LA, R,
        C,  R,      LB, C,      LA, R,
        C,  R,      LB, C,      LA, R,


        E,  R,      D,  E,      C,  R,
        E,  R,      D,  E,      C,  R,
        E,  R,      D,  E,      C,  R,
        E,  R,      D,  E,      C,  R,

        A,  R,      A,  A,      G,  F,
        E,  R,      E,  E,      D,  C,
        D,  R,      D,  D,      E,  D,
        C,  R,      LB, C,      LA, R,

        LE, LFSH,   LGSH, LA,   LB, C,
        D,  E,      D,  R,      C,  R,
        LE, LFSH,   LGSH, LA,   LB, C,
        D,  E,      D,  R,      C,  R,


        C,  R,      LB, C,      LA, R, // back to main theme (but only for 8 measures)
        C,  R,      LB, C,      LA, R,
        C,  R,      LB, C,      LA, R,
        C,  R,      LB, C,      LA, R,

        C,  R,      LB, C,      LA, R,
        C,  R,      LB, C,      LA, R,
        C,  R,      LB, C,      LA, R,
        C,  R,      LB, C,      LA, R,

        E,  R,      D,  E,      LA,  R,
        R,  R,      R,  R,      R,  R
    };


    // Missing HighE, HighD, LowB, C#, D#, F#
    // Not using F, C, or D
    // 11 bells in total
    int carolInMidG[NUM_NOTES] =
    {
        G,  R,      FS, G,      E,  R, // 12 measures of main theme
        G,  R,      FS, G,      E,  R,
        G,  R,      FS, G,      E,  R,
        G,  R,      FS, G,      E,  R,

        G,  R,      FS, G,      E,  R,
        G,  R,      FS, G,      E,  R,
        G,  R,      FS, G,      E,  R,
        G,  R,      FS, G,      E,  R,

        G,  R,      FS, G,      E,  R,
        G,  R,      FS, G,      E,  R,
        G,  R,      FS, G,      E,  R,
        G,  R,      FS, G,      E,  R,


        B,  R,      A,  B,      G,  R,
        B,  R,      A,  B,      G,  R,
        B,  R,      A,  B,      G,  R,
        B,  R,      A,  B,      G,  R,

        HE,  R,     HE, HE,     HD, HC,
        B,  R,      B,  B,      A,  G,
        A,  R,      A,  A,      B,  A,
        G,  R,      FS, G,      E,  R,

        LB, CS,     DS, E,      FS, G,
        A,  B,      A,  R,      G,  R,
        LB, CS,     DS, E,      FS, G,
        A,  B,      A,  R,      G,  R,


        G,  R,      FS, G,      E,  R, // back to main theme (but only for 8 measures)
        G,  R,      FS, G,      E,  R,
        G,  R,      FS, G,      E,  R,
        G,  R,      FS, G,      E,  R,

        G,  R,      FS, G,      E,  R,
        G,  R,      FS, G,      E,  R,
        G,  R,      FS, G,      E,  R,
        G,  R,      FS, G,      E,  R,

        B,  R,      A,  B,      E,  R,
        R,  R,      R,  R,      R,  R
    };



    // Missing HighD, HighE, HighF, HighG, HighA, G#, F#
    // Not using C, D, E, F, G
    // Uses 11 bells in total
    int carolInHighC[NUM_NOTES] =
    {
        HC, R,      B, HC,      A, R, // 12 measures of main theme
        HC, R,      B, HC,      A, R,
        HC, R,      B, HC,      A, R,
        HC, R,      B, HC,      A, R,

        HC, R,      B, HC,      A, R,
        HC, R,      B, HC,      A, R,
        HC, R,      B, HC,      A, R,
        HC, R,      B, HC,      A, R,

        HC, R,      B, HC,      A, R,
        HC, R,      B, HC,      A, R,
        HC, R,      B, HC,      A, R,
        HC, R,      B, HC,      A, R,


        HE, R,      HD, HE,     HC, R,
        HE, R,      HD, HE,     HC, R,
        HE, R,      HD, HE,     HC, R,
        HE, R,      HD, HE,     HC, R,

        HA, R,      HA, HA,     HG, HF,
        HE, R,      HE, HE,     HD, HC,
        HD, R,      HD, HD,     HE, HD,
        HC, R,      B,  HC,     A, R,

        E, FSH,     GSH, A,     B, HC,
        HD, HE,     HD,  R,     HC, R,
        E, FSH,     GSH, A,     B, HC,
        HD,  HE,    HD,  R,     HC,  R,


        HC, R,      B, HC,      A, R, // back to main theme (but only for 8 measures)
        HC, R,      B, HC,      A, R,
        HC, R,      B, HC,      A, R,
        HC, R,      B, HC,      A, R,

        HC, R,      B, HC,      A, R,
        HC, R,      B, HC,      A, R,
        HC, R,      B, HC,      A, R,
        HC, R,      B, HC,      A, R,

        HE, R,      HD, HE,     A,  R,
        R,  R,      R,  R,      R,  R
    };







}