
#include "phylip.h"
#include "seq.h"

/* version 3.6. (c) Copyright 1986-2000 by the University of Washington
  and by Joseph Felsenstein.  Written by Joseph Felsenstein and Lucas Mix.
  Permission is granted to copy and use this program provided no fee is
  charged for it and provided that this copyright notice is not removed. */

#define epsilon         0.0001   /* used in makenewv, getthree, update */
#define over            60

typedef long vall[maxcategs];
typedef double contribarr[maxcategs];

AjPSeqset* seqsets = NULL;
AjPPhyloProp phyloratecat = NULL;
AjPPhyloProp phyloweights = NULL;
AjPPhyloTree* phylotrees;

#ifndef OLDC
/* function prototypes */
void   init_protmats(void);
/*void   getoptions(void);*/
void emboss_getoptions(char *pgm, int argc, char *argv[]);
void   makeprotfreqs(void);
void   allocrest(void); 
void   doinit(void);
void   inputoptions(void);
void   input_protdata(AjPSeqset, long);
void   makeweights(void);
void   prot_makevalues(long, pointarray, long, long, sequence, steptr);
void   getinput(void);

void   prot_inittable(void);
void   alloc_pmatrix(long);
void   make_pmatrix(double **, double **, double **, long, double, double,
                double *, double **);
void   prot_nuview(node *);
void   getthree(node *p, double thigh, double tlow);
void   makenewv(node *);
void   update(node *);
void   smooth(node *);
void   promlk_add(node *, node *, node *, boolean);
void   promlk_re_move(node **, node **, boolean);

double prot_evaluate(node *);
void   tryadd(node *, node **, node **);
void   addpreorder(node *, node *, node *, boolean, boolean);
void   restoradd(node *, node *, node *, double);
void   tryrearr(node *, boolean *); 
void   repreorder(node *, boolean *);
void   rearrange(node **);
void   nodeinit(node *);
void   initrav(node *);
void   travinit(node *);

void   travsp(node *);
void   treevaluate(void);
void   promlk_coordinates(node *, long *);
void   promlk_drawline(long, double);
void   promlk_printree(void);
void   describe(node *);
void   prot_reconstr(node *, long);
void   rectrav(node *, long, long);
void   summarize(void);
void   promlk_treeout(node *);
void   initpromlnode(node **, node **, node *, long, long, long *, long *,
                initops, pointarray, pointarray, Char *, Char *, char**);
void   tymetrav(node *, double *);

void   free_all_protx(long, pointarray);
void   maketree(void);
void   clean_up(void);
/* function prototypes */
#endif


extern sequence y;
double **tbl;

Char infilename[100], intreename[100],
     catfilename[100], weightfilename[100];
const char* outfilename;
const char* outtreename;
double *rrate;
long sites, weightsum, categs, datasets, ith, njumble, jumb, numtrees;
/*  sites = number of sites in actual sequences
  numtrees = number of user-defined trees */
long inseed, inseed0, mx, mx0, mx1;
boolean global, jumble, lngths, trout, usertree, weights, rctgry, ctgry,
                  auto_, progress, mulsets, firstset, hypstate, reconsider,
                  smoothit, polishing, justwts, gama, invar, usepam, usejtt;
tree curtree, bestree, bestree2;
node *qwhere, *grbg;
double sumrates, cv, alpha, lambda, lambda1, invarfrac;
long *enterorder;
steptr aliasweight;
double *rate;
longer seed;
double *probcat;
contribarr *contribution;
char aachar[26]="ARNDCQEGHILKMFPSTWYVBZX?*-";
char *progname;
long rcategs, nonodes2;


/* Local variables for maketree, propagated globally for C version: */
long    k, maxwhich, col;
double  like, bestyet, tdelta, lnlike, slope, curv, maxlogl;
boolean lastsp, smoothed, succeeded;
double  *l0gl;
double  x[3], lnl[3];
double  expon1i[maxcategs], expon1v[maxcategs],
        expon2i[maxcategs], expon2v[maxcategs];
node   *there;
double  **l0gf;
Char ch, ch2;
vall *mp;


/* Variables introduced to allow for protein probability calculations   */
long   max_num_sibs;            /* maximum number of siblings used in a */
                                /* nuview calculation.  determines size */
                                /* final size of pmatrices              */
double *eigmat;                 /* eig matrix variable                  */
double **probmat;               /* prob matrix variable                 */
double ****dpmatrix;            /* derivative of pmatrix                */
double ****ddpmatrix;           /* derivative of xpmatrix               */
double *****pmatrices;          /* matrix of probabilities of protien   */
                                /* conversion.  The 5 subscripts refer  */
                                /* to sibs, rcategs, categs, final and  */
                                /* initial states, respectively.        */
double freqaa[20];              /* amino acid frequencies               */
       
static double pameigmat[20] =
    {
       -0.022091252,-0.019297602, 0.000004760,-0.017477817,
       -0.016575549,-0.015504543,-0.002112213,-0.002685727,
       -0.002976402,-0.013440755,-0.012926992,-0.004293227,
       -0.005356688,-0.011064786,-0.010480731,-0.008760449,
       -0.007142318,-0.007381851,-0.007806557,-0.008127024
    };

static double pamprobmat[20][20] =
    {
      {-0.01522976,-0.00746819,-0.13934468, 0.11755315,-0.00212101,
        0.01558456,-0.07408235,-0.00322387, 0.01375826, 0.00448826,
        0.00154174, 0.02013313,-0.00159183,-0.00069275,-0.00399898,
        0.08414055,-0.01188178,-0.00029870, 0.00220371, 0.00042546},
      {-0.07765582,-0.00712634,-0.03683209,-0.08065755,-0.00462872,
       -0.03791039, 0.10642147,-0.00912185, 0.01436308,-0.00133243,
        0.00166346, 0.00624657,-0.00003363,-0.00128729,-0.00690319,
        0.17442028,-0.05373336,-0.00078751,-0.00038151, 0.01382718},
      {-0.08810973,-0.04081786,-0.04066004,-0.04736004,-0.03275406,
       -0.03761164,-0.05047487,-0.09086213,-0.03269598,-0.03558015,
       -0.08407966,-0.07970977,-0.01504743,-0.04011920,-0.05182232,
       -0.07026991,-0.05846931,-0.01016998,-0.03047472,-0.06280511},
      { 0.02513756,-0.00578333, 0.09865453, 0.01322314,-0.00310665,
        0.05880899,-0.09252443,-0.02986539,-0.03127460, 0.01007539,
       -0.00360119,-0.01995024, 0.00094940,-0.00145868,-0.01388816,
        0.11358341,-0.12127513,-0.00054696,-0.00055627, 0.00417284},
      { 0.16517316,-0.00254742,-0.03318745,-0.01984173, 0.00031890,
       -0.02817810, 0.02661678,-0.01761215, 0.01665112, 0.10513343,
       -0.00545026, 0.01827470,-0.00207616,-0.00763758,-0.01322808,
       -0.02202576,-0.07434204, 0.00020593, 0.00119979,-0.10827873},
      { 0.16088826, 0.00056313,-0.02579303,-0.00319655, 0.00037228,
       -0.03193150, 0.01655305,-0.03028640, 0.01367746,-0.11248153,
        0.00778371, 0.02675579, 0.00243718, 0.00895470,-0.01729803,
       -0.02686964,-0.08262584, 0.00011794,-0.00225134, 0.09415650},
      {-0.01739295, 0.00572017,-0.00712592,-0.01100922,-0.00870113,
       -0.00663461,-0.01153857,-0.02248432,-0.00382264,-0.00358612,
       -0.00139345,-0.00971460,-0.00133312, 0.01927783,-0.01053838,
       -0.00911362,-0.01010908, 0.09417598, 0.01763850,-0.00955454},
      { 0.01728888, 0.01344211, 0.01200836, 0.01857259,-0.17088517,
        0.01457592, 0.01997839, 0.02844884, 0.00839403, 0.00196862,
        0.01391984, 0.03270465, 0.00347173,-0.01940984, 0.01233979,
        0.00542887, 0.01008836, 0.00126491,-0.02863042, 0.00449764},
      {-0.02881366,-0.02184155,-0.01566086,-0.02593764,-0.04050907,
       -0.01539603,-0.02576729,-0.05089606,-0.00597430, 0.02181643,
        0.09835597,-0.04040940, 0.00873512, 0.12139434,-0.02427882,
       -0.02945238,-0.01566867,-0.01606503, 0.09475319, 0.02238670},
      { 0.04080274,-0.02869626,-0.05191093,-0.08435843, 0.00021141,
        0.13043842, 0.00871530, 0.00496058,-0.02797641,-0.00636933,
        0.02243277, 0.03640362,-0.05735517, 0.00196918,-0.02218934,
       -0.00608972, 0.02872922, 0.00047619, 0.00151285, 0.00883489},
      {-0.02623824, 0.00331152, 0.03640692, 0.04260231,-0.00038223,
       -0.07480340,-0.01022492,-0.00426473, 0.01448116, 0.01456847,
        0.05786680, 0.03368691,-0.10126924,-0.00147454, 0.01275395,
        0.00017574,-0.01585206,-0.00015767,-0.00231848, 0.02310137},
      {-0.00846258,-0.01508106,-0.01967505,-0.02772004, 0.01248253,
       -0.01331243,-0.02569382,-0.04461524,-0.02207075, 0.04663443,
        0.19347923,-0.02745691, 0.02288515,-0.04883849,-0.01084597,
       -0.01947187,-0.00081675, 0.00516540,-0.07815919, 0.08035585},
      {-0.06553111, 0.09756831, 0.00524326,-0.00885098, 0.00756653,
        0.02783099,-0.00427042,-0.16680359, 0.03951331,-0.00490540,
        0.01719610, 0.15018204, 0.00882722,-0.00423197,-0.01919217,
       -0.02963619,-0.01831342,-0.00524338, 0.00011379,-0.02566864},
      {-0.07494341,-0.11348850, 0.00241343,-0.00803016, 0.00492438,
        0.00711909,-0.00829147, 0.05793337, 0.02734209, 0.02059759,
       -0.02770280, 0.14128338, 0.01532479, 0.00364307, 0.05968116,
       -0.06497960,-0.08113941, 0.00319445,-0.00104222, 0.03553497},
      { 0.05948223,-0.08959930, 0.03269977,-0.03272374,-0.00365667,
       -0.03423294,-0.06418925,-0.05902138, 0.05746317,-0.02580596,
        0.01259572, 0.05848832, 0.00672666, 0.00233355,-0.05145149,
        0.07348503, 0.11427955, 0.00142592,-0.01030651,-0.04862799},
      {-0.01606880, 0.05200845,-0.01212967,-0.06824429,-0.00234304,
        0.01094203,-0.07375538, 0.08808629, 0.12394822, 0.02231351,
       -0.03608265,-0.06978045,-0.00618360, 0.00274747,-0.01921876,
       -0.01541969,-0.02223856,-0.00107603,-0.01251777, 0.05412534},
      { 0.01688843, 0.05784728,-0.02256966,-0.07072251,-0.00422551,
       -0.06261233,-0.08502830, 0.08925346,-0.08529597, 0.01519343,
       -0.05008258, 0.10931873, 0.00521033, 0.02593305,-0.00717855,
        0.02291527, 0.02527388,-0.00266188,-0.00871160, 0.02708135},
      {-0.04233344, 0.00076379, 0.01571257, 0.04003092, 0.00901468,
        0.00670577, 0.03459487, 0.12420216,-0.00067366,-0.01515094,
        0.05306642, 0.04338407, 0.00511287, 0.01036639,-0.17867462,
       -0.02289440,-0.03213205, 0.00017924,-0.01187362,-0.03933874},
      { 0.01284817,-0.01685622, 0.00724363, 0.01687952,-0.00882070,
       -0.00555957, 0.01676246,-0.05560456,-0.00966893, 0.06197684,
       -0.09058758, 0.00880607, 0.00108629,-0.08308956,-0.08056832,
       -0.00413297, 0.02973107, 0.00092948, 0.07010111, 0.13007418},
      { 0.00700223,-0.01347574, 0.00691332, 0.03122905, 0.00310308,
        0.00946862, 0.03455040,-0.06712536,-0.00304506, 0.04267941,
       -0.10422292,-0.01127831,-0.00549798, 0.11680505,-0.03352701,
       -0.00084536, 0.01631369, 0.00095063,-0.09570217, 0.06480321}
    };

/* this jtt matrix decomposition due to Elisabeth  Tillier */
static double jtteigmat[] =
{0.0,        -0.007031123, -0.006484345, -0.006086499, -0.005514432,
-0.00772664, -0.008643413, -0.010620756, -0.009965552, -0.011671808,
-0.012222418,-0.004589201, -0.013103714, -0.014048038, -0.003170582,
-0.00347935, -0.015311677, -0.016021194, -0.017991454, -0.018911888};

static double jttprobmat[20][20] =
{{0.076999996, 0.051000003, 0.043000004, 0.051999998, 0.019999996, 0.041,
  0.061999994, 0.073999997, 0.022999999, 0.052000004, 0.090999997, 0.058999988,
  0.024000007, 0.04, 0.050999992, 0.069, 0.059000006, 0.014000008, 0.032000004,
  0.066000005},
 {0.015604455, -0.068062363, 0.020106264, 0.070723273, 0.011702977, 0.009674053,
  0.074000798, -0.169750458, 0.005560808, -0.008208636, -0.012305869,
 -0.063730179, -0.005674643, -0.02116828, 0.104586169, 0.016480839, 0.016765139,
  0.005936994, 0.006046367, -0.0082877},
 {-0.049778281, -0.007118197, 0.003801272, 0.070749616, 0.047506147,
   0.006447017, 0.090522425, -0.053620432, -0.008508175, 0.037170603,
   0.051805545, 0.015413608, 0.019939916, -0.008431976, -0.143511376,
  -0.052486072, -0.032116542, -0.000860626, -0.02535993, 0.03843545},
 {-0.028906423, 0.092952047, -0.009615343, -0.067870117, 0.031970392,
   0.048338335, -0.054396304, -0.135916654, 0.017780083, 0.000129242,
   0.031267424, 0.116333586, 0.007499746, -0.032153596, 0.033517051,
  -0.013719269, -0.00347293, -0.003291821, -0.02158326, -0.008862168},
 {0.037181176, -0.023106564, -0.004482225, -0.029899635, 0.118139633,
 -0.032298569, -0.04683198, 0.05566988, -0.012622847, 0.002023096,
 -0.043921088, -0.04792557, -0.003452711, -0.037744513, 0.020822974,
  0.036580187, 0.02331425, -0.004807711, -0.017504496, 0.01086673},
 {0.044754061, -0.002503471, 0.019452517, -0.015611487, -0.02152807,
 -0.013131425, -0.03465365, -0.047928912, 0.020608851, 0.067843095,
 -0.122130014, 0.002521499, 0.013021646, -0.082891087, -0.061590119,
  0.016270856, 0.051468938, 0.002079063, 0.081019713, 0.082927944},
 {0.058917882, 0.007320741, 0.025278141, 0.000357541, -0.002831285,
 -0.032453034, -0.010177288, -0.069447924, -0.034467324, 0.011422358,
 -0.128478324, 0.04309667, -0.015319944, 0.113302422, -0.035052393,
  0.046885372, 0.06185183, 0.00175743, -0.06224497, 0.020282093},
 {-0.014562092, 0.022522921, -0.007094389, 0.03480089, -0.000326144,
 -0.124039037, 0.020577906, -0.005056454, -0.081841576, -0.004381786,
  0.030826152, 0.091261631, 0.008878828, -0.02829487, 0.042718836,
 -0.011180886, -0.012719227, -0.000753926, 0.048062375, -0.009399129},
 {0.033789571, -0.013512235, 0.088010984, 0.017580292, -0.006608005,
 -0.037836971, -0.061344686, -0.034268357, 0.018190209, -0.068484614,
  0.120024744, -0.00319321, -0.001349477, -0.03000546, -0.073063759,
  0.081912399, 0.0635245, 0.000197, -0.002481798, -0.09108114},
 {-0.113947615, 0.019230545, 0.088819683, 0.064832765, 0.001801467,
 -0.063829682, -0.072001633, 0.018429333, 0.057465965, 0.043901014,
 -0.048050874, -0.001705918, 0.022637173, 0.017404665, 0.043877902,
 -0.017089594, -0.058489485, 0.000127498, -0.029357194, 0.025943972},
 {0.01512923, 0.023603725, 0.006681954, 0.012360216, -0.000181447,
 -0.023011838, -0.008960024, -0.008533239, 0.012569835, 0.03216118,
  0.061986403, -0.001919083, -0.1400832, -0.010669741, -0.003919454,
 -0.003707024, -0.026806029, -0.000611603, -0.001402648, 0.065312824},
 {-0.036405351, 0.020816769, 0.011408213, 0.019787053, 0.038897829,
   0.017641789, 0.020858533, -0.006067252, 0.028617353, -0.064259496,
  -0.081676567, 0.024421823, -0.028751676, 0.07095096, -0.024199434,
  -0.007513119, -0.028108766, -0.01198095, 0.111761119, -0.076198809},
 {0.060831772, 0.144097327, -0.069151377, 0.023754576, -0.003322955,
 -0.071618574, 0.03353154, -0.02795295, 0.039519769, -0.023453968,
 -0.000630308, -0.098024591, 0.017672997, 0.003813378, -0.009266499,
 -0.011192111, 0.016013873, -0.002072968, -0.010022044, -0.012526904},
 {-0.050776604, 0.092833081, 0.044069596, 0.050523021, -0.002628417,
   0.076542572, -0.06388631, -0.00854892, -0.084725311, 0.017401063,
  -0.006262541, -0.094457679, -0.002818678, -0.0044122, -0.002883973,
   0.028729685, -0.004961596, -0.001498627, 0.017994575, -0.000232779},
 {-0.01894566, -0.007760205, -0.015160993, -0.027254587, 0.009800903,
  -0.013443561, -0.032896517, -0.022734138, -0.001983861, 0.00256111,
   0.024823166, -0.021256768, 0.001980052, 0.028136263, -0.012364384,
  -0.013782446, -0.013061091, 0.111173981, 0.021702122, 0.00046654},
 {-0.009444193, -0.042106824, -0.02535015, -0.055125574, 0.006369612,
  -0.02945416, -0.069922064, -0.067221068, -0.003004999, 0.053624311,
   0.128862984, -0.057245803, 0.025550508, 0.087741073, -0.001119043,
  -0.012036202, -0.000913488, -0.034864475, 0.050124813, 0.055534723},
 {0.145782464, -0.024348311, -0.031216873, 0.106174443, 0.00202862,
  0.02653866, -0.113657267, -0.00755018, 0.000307232, -0.051241158,
  0.001310685, 0.035275877, 0.013308898, 0.002957626, -0.002925034,
 -0.065362319, -0.071844582, 0.000475894, -0.000112419, 0.034097762},
 {0.079840455, 0.018769331, 0.078685899, -0.084329807, -0.00277264,
 -0.010099754, 0.059700608, -0.019209715, -0.010442992, -0.042100476,
 -0.006020556, -0.023061786, 0.017246106, -0.001572858, -0.006703785,
  0.056301316, -0.156787357, -0.000303638, 0.001498195, 0.051363455},
 {0.049628261, 0.016475144, 0.094141653, -0.04444633, 0.005206131,
 -0.001827555, 0.02195624, 0.013066683, -0.010415582, -0.022338403,
  0.007837197, -0.023397671, -0.002507095, 0.005177694, 0.017109561,
 -0.202340113, 0.069681441, 0.000120736, 0.002201146, 0.004670849},
 {0.089153689, 0.000233354, 0.010826822, -0.004273519, 0.001440618,
  0.000436077, 0.001182351, -0.002255508, -0.000700465, 0.150589876,
 -0.003911914, -0.00050154, -0.004564983, 0.00012701, -0.001486973,
 -0.018902754, -0.054748555, 0.000217377, -0.000319302, -0.162541651}};


void init_protmats()
{
  long l, m;

  eigmat = (double *) Malloc (20 * sizeof(double));
  for (l = 0; l <= 19; l++)
    if (usejtt)
      eigmat[l] = jtteigmat[l];
    else
      eigmat[l] = pameigmat[l];
  probmat = (double **) Malloc (20 * sizeof(double *));
  for (l = 0; l < 20; l++)
    probmat[l] = (double *) Malloc (20 * sizeof(double));
  for (l = 0; l <= 19; l++)
    for (m= 0; m <= 19; m++)
      if (usejtt)
        probmat[l][m] = jttprobmat[l][m];
      else
        probmat[l][m] = pamprobmat[l][m];
}  /* init_protmats */



/************ EMBOSS GET OPTIONS ROUTINES ******************************/

void emboss_getoptions(char *pgm, int argc, char *argv[])
{
    AjStatus retval;
 
    /* initialize global variables */

    ajNamInit("emboss");
    retval =  ajAcdInitP (pgm, argc, argv, "PHYLIP");

    /* ajAcdGet */

    /* init functions for standard ajAcdGet */

    /* cleanup for clashing options */

}

/************ END EMBOSS GET OPTIONS ROUTINES **************************/

/*
//void getoptions()
//{
//  /# interactively set options #/
//  long i, loopcount, loopcount2;
//  Char ch;
//  boolean done;
//  boolean didchangecat, didchangercat;
//  double probsum;
// 
//  fprintf(outfile, "\nAmino acid sequence\n");
//  fprintf(outfile, "   Maximum Likelihood method with molecular ");
//  fprintf(outfile, "clock, version %s\n\n", VERSION);
//
//  putchar('\n');
//  auto_ = false;
//  ctgry = false;
//  didchangecat = false;
//  rctgry = false;
//  didchangercat = false;
//  categs = 1;
//  rcategs = 1;
//  gama = false;
//  global = false;
//  hypstate = false;
//  invar = false;
//  jumble = false;
//  njumble = 1;
//  lambda = 1.0;
//  lambda1 = 0.0;
//  lngths = false;
//  trout = true;
//  usepam = false;
//  usejtt = true;
//  usertree = false;
//  weights = false;
//  printdata = false;
//  progress = true;
//  treeprint = true;
//  interleaved = true;
//  init_protmats();
//  loopcount = 0;
//  do {
//    cleerhome();
//    printf("\nAmino acid sequence\n");
//    printf("   Maximum Likelihood method with molecular clock, version %s\n\n",
//           VERSION);
//    printf("Settings for this run:\n");
//    printf("  U                 Search for best tree?");
//    if (usertree)
//     printf("  No, use user trees in input file\n");
//    else
//      printf("  Yes\n");
//    printf("  P   JTT or PAM amino acid change model?  %s\n",
//           (usejtt ? "Jones-Taylor-Thornton model" : "Dayhoff PAM model"));
//    if (usertree) {
//      printf("  L           Use lengths from user tree?");
//      if (lngths)
//        printf("  Yes\n");
//      else
//        printf("  No\n");
//    }
//    printf("  C   One category of substitution rates?");
//    if (!ctgry)
//      printf("  Yes\n");
//    else
//      printf("  %ld categories\n", categs);
//    printf("  R           Rate variation among sites?");
//    if (!rctgry)
//      printf("  constant rate of change\n");
//    else {
//      if (gama)
//        printf("  Gamma distributed rates\n");
//      else {
//        if (invar)
//          printf("  Gamma+Invariant sites\n");
//        else
//          printf("  user-defined HMM of rates\n");
//      }
//      printf("  A   Rates at adjacent sites correlated?");
//      if (!auto_)
//        printf("  No, they are independent\n");
//      else
//        printf("  Yes, mean block length =%6.1f\n", 1.0 / lambda);
//    }
//    if (!usertree) {
//      printf("  G                Global rearrangements?");
//      if (global)
//        printf("  Yes\n");
//      else
//        printf("  No\n");
//    }
//    printf("  W                       Sites weighted?  %s\n",
//           (weights ? "Yes" : "No"));
//    if (!usertree) {
//      printf("  J   Randomize input order of sequences?");
//      if (jumble)
//        printf("  Yes (seed = %8ld, %3ld times)\n", inseed0, njumble);
//      else
//        printf("  No. Use input order\n");
//    }
//    printf("  M           Analyze multiple data sets?");
//    if (mulsets)
//      printf("  Yes, %2ld %s\n", datasets,
//               (justwts ? "sets of weights" : "data sets"));
//    else
//      printf("  No\n");
//    printf("  I          Input sequences interleaved?");
//    if (interleaved)
//      printf("  Yes\n");
//    else
//      printf("  No, sequential\n");
//    printf("  0   Terminal type (IBM PC, ANSI, none)?");
//    if (ibmpc)
//      printf("  IBM PC\n");
//    if (ansi)
//      printf("  ANSI\n");
//    if (!(ibmpc || ansi))
//      printf("  (none)\n");
//    printf("  1    Print out the data at start of run");
//    if (printdata)
//      printf("  Yes\n");
//    else
//      printf("  No\n");
//    printf("  2  Print indications of progress of run");
//    if (progress)
//      printf("  Yes\n");
//    else
//      printf("  No\n");
//    printf("  3                        Print out tree");
//    if (treeprint)
//      printf("  Yes\n");
//    else
//      printf("  No\n");
//    printf("  4       Write out trees onto tree file?");
//    if (trout)
//      printf("  Yes\n");
//    else
//      printf("  No\n");
//    printf("  5   Reconstruct hypothetical sequences?  %s\n",
//           (hypstate ? "Yes" : "No"));
//    printf("\nAre these settings correct? ");
//    printf("(type Y or the letter for one to change)\n");
//#ifdef WIN32
//    phyFillScreenColor();
//#endif
//    scanf("%c%*[^\n]", &ch);
//    getchar();
//    if (ch == '\n')
//      ch = ' ';
//    uppercase(&ch);
//    done = (ch == 'Y');
//    if (!done) {
//      uppercase(&ch);
//      if (strchr("UPCRJAFWGLTMI012345", ch) != NULL){
//        switch (ch) {
//
//        case 'C':
//          ctgry = !ctgry;
//          if (ctgry) {
//            printf("\nSitewise user-assigned categories:\n\n");
//            initcatn(&categs);
//            if (rate){
//              free(rate);
//            }
//            rate    = (double *) Malloc( categs * sizeof(double));
//            didchangecat = true;
//            initcategs(categs, rate);
//          }
//          break;
//     
//        case 'P':
//          if (usejtt) {
//            usejtt = false;
//            usepam = true;
//          } else {
//              usepam = false;
//              usejtt = true;
//          }
//          break;
//  
//        case 'R':
//          if (!rctgry) {
//            rctgry = true;
//            gama = true;
//          } else {
//            if (gama) {
//              gama = false;
//              invar = true;
//            } else {
//              if (invar)
//                invar = false;
//              else
//                rctgry = false;
//            }
//          }
//          break;
//     
//        case 'A':
//          auto_ = !auto_;
//          if (auto_) {
//            initlambda(&lambda);
//            lambda1 = 1.0 - lambda;
//          }
//          break;
//     
//        case 'G':
//          global = !global;
//          break;
//     
//        case 'W':
//          weights = !weights;
//          break;
//
//        case 'J':
//          jumble = !jumble;
//          if (jumble)
//            initjumble(&inseed, &inseed0, seed, &njumble);
//          else njumble = 1;
//          break;
//     
//        case 'L':
//          lngths = !lngths;
//          break;
//     
//        case 'U':
//          usertree = !usertree;
//          break;
//     
//        case 'M':
//          mulsets = !mulsets;
//          if (mulsets) {
//            printf("Multiple data sets or multiple weights?");
//            loopcount2 = 0;
//            do {
//              printf(" (type D or W)\n");
//#ifdef WIN32
//              phyFillScreenColor();
//#endif
//              scanf("%c%*[^\n]", &ch2);
//              getchar();
//              if (ch2 == '\n')
//                  ch2 = ' ';
//              uppercase(&ch2);
//              countup(&loopcount2, 10);
//            } while ((ch2 != 'W') && (ch2 != 'D'));
//            justwts = (ch2 == 'W');
//            if (justwts)
//              justweights(&datasets);
//            else
//              initdatasets(&datasets);
//            if (!jumble) {
//              jumble = true;
//              initjumble(&inseed, &inseed0, seed, &njumble);
//            }
//          }
//          break;
//     
//        case 'I':
//          interleaved = !interleaved;
//          break;
//     
//        case '0':
//          initterminal(&ibmpc, &ansi);
//          break;
//     
//        case '1':
//          printdata = !printdata;
//          break;
//     
//        case '2':
//          progress = !progress;
//          break;
//     
//        case '3':
//          treeprint = !treeprint;
//          break;
//     
//        case '4':
//          trout = !trout;
//          break;
//
//        case '5':
//          hypstate = !hypstate;
//          break;
//        }
//      } else
//        printf("Not a possible option!\n");
//    }
//    countup(&loopcount, 100);
//  } while (!done);
//  if (gama || invar) {
//    loopcount = 0;
//    do {
//      printf(
//"\nCoefficient of variation of substitution rate among sites (must be positive)\n");
//      printf(
//  " In gamma distribution parameters, this is 1/(square root of alpha)\n");
//#ifdef WIN32
//      phyFillScreenColor();
//#endif
//      scanf("%lf%*[^\n]", &cv);
//      getchar();
//      countup(&loopcount, 10);
//    } while (cv <= 0.0);
//    alpha = 1.0 / (cv * cv);
//  }
//  if (!rctgry)
//    auto_ = false;
//  if (rctgry) {
//    printf("\nRates in HMM");
//    if (invar)
//      printf(" (including one for invariant sites)");
//    printf(":\n");
//    initcatn(&rcategs);
//    if (probcat){
//      free(probcat);
//      free(rrate);
//    }
//    probcat = (double *) Malloc(rcategs * sizeof(double));
//    rrate   = (double *) Malloc(rcategs * sizeof(double));
//    didchangercat = true;
//    if (gama)
//      initgammacat(rcategs, alpha, rrate, probcat); 
//    else {
//      if (invar) {
//        loopcount = 0;
//        do {
//          printf("Fraction of invariant sites?\n");
//          scanf("%lf%*[^\n]", &invarfrac);
//          getchar();
//          countup (&loopcount, 10);
//        } while ((invarfrac <= 0.0) || (invarfrac >= 1.0));
//        initgammacat(rcategs-1, alpha, rrate, probcat); 
//        for (i = 0; i < rcategs-1; i++)
//          probcat[i] = probcat[i]*(1.0-invarfrac);
//        probcat[rcategs-1] = invarfrac;
//        rrate[rcategs-1] = 0.0;
//      } else {
//        initcategs(rcategs, rrate);
//        initprobcat(rcategs, &probsum, probcat);
//      }
//    }
//  }
//  if (!didchangercat){
//    rrate      = Malloc( rcategs*sizeof(double));
//    probcat    = Malloc( rcategs*sizeof(double));
//    rrate[0]   = 1.0;
//    probcat[0] = 1.0;
//  }  
//  if (!didchangecat){
//    rate       = Malloc( categs*sizeof(double));
//    rate[0]    = 1.0;
//  }  
//}  /# getoptions #/
*/


void makeprotfreqs()
{
  /* calculate amino acid frequencies based on eigmat */
  long i, mineig;

  mineig = 0;
  for (i = 0; i <= 19; i++)
    if (fabs(eigmat[i]) < fabs(eigmat[mineig]))
      mineig = i;
  memcpy(freqaa, probmat[mineig], 20 * sizeof(double));
  for (i = 0; i <= 19; i++)
    freqaa[i] = fabs(freqaa[i]);
} /* makeprotfreqs */


void allocrest()
{
  long i;

  y     = (Char **)Malloc(spp*sizeof(Char *));
  nayme  = (naym *)Malloc(spp*sizeof(naym));
  for (i = 0; i < spp; i++)
    y[i] = (char *)Malloc(sites * sizeof(char));
  enterorder  = (long *)Malloc(spp*sizeof(long));
  weight      = (long *)Malloc(sites*sizeof(long));
  category    = (long *)Malloc(sites*sizeof(long));
  alias       = (long *)Malloc(sites*sizeof(long));
  aliasweight = (long *)Malloc(sites*sizeof(long));
  ally        = (long *)Malloc(sites*sizeof(long));
  location    = (long *)Malloc(sites*sizeof(long));
}  /* allocrest */


void doinit()
{
  /* initializes variables */

  inputnumbersseq(seqsets[0], &spp, &sites, &nonodes, 1);
/*  getoptions();*/
  makeprotfreqs();
  if (printdata)
    fprintf(outfile, "%2ld species, %3ld  sites\n", spp, sites);
  alloctree(&curtree.nodep, nonodes, usertree);
  allocrest();
  if (usertree)
    return;
  alloctree(&bestree.nodep, nonodes, 0);
  if (njumble <= 1)
    return;
  alloctree(&bestree2.nodep, nonodes, 0);
}  /* doinit */


void inputoptions()
{
  long i;

  if (!firstset)
    samenumspseq(seqsets[ith-1], &sites, ith);
  if (firstset) {
    for (i = 0; i < sites; i++)
      category[i] = 1;
    for (i = 0; i < sites; i++)
      weight[i] = 1;
  }
  if (justwts || weights)
    inputweightsstr(phyloweights->Str[0], sites, weight, &weights);
  weightsum = 0;
  for (i = 0; i < sites; i++)
    weightsum += weight[i];
  if ((ctgry && categs > 1) && (firstset || !justwts)) {
    inputcategsstr(phyloratecat->Str[0], 0, sites, category, categs, "ProMLK");
    if (printdata)
      printcategs(outfile, sites, category, "Site categories");
  }
  if (weights && printdata)
    printweights(outfile, 0, sites, weight, "Sites");
  fprintf(outfile, "%s model of amino acid change\n\n",
          (usejtt ? "Jones-Taylor-Thornton" : "Dayhoff PAM"));
}  /* inputoptions */


void input_protdata(AjPSeqset seqset, long chars)
{
  /* input the names and sequences for each species */
  /* used by proml */
  long i, j, k, l;
  Char charstate;

  if (printdata)
    headings(chars, "Sequences", "---------");
  for(i=0;i<spp;i++){
    strncpy(&nayme[i][0],ajStrStr(ajSeqsetName(seqset, i)),nmlngth);
    /*    ajUser("%s/n",ajSeqsetName(seqset, i));*/
    strncpy(&y[i][0],ajSeqsetSeq(seqset, i),chars);
    y[i][chars] = '\0';
  }
  if (!printdata)
    return;
  for (i = 1; i <= ((chars - 1) / 60 + 1); i++) {
    for (j = 1; j <= spp; j++) {
      for (k = 0; k < nmlngth; k++)
        putc(nayme[j - 1][k], outfile);
      fprintf(outfile, "   ");
      l = i * 60;
      if (l > chars)
        l = chars;
      for (k = (i - 1) * 60 + 1; k <= l; k++) {
        if (j > 1 && y[j - 1][k - 1] == y[0][k - 1])
          charstate = '.';
        else
          charstate = y[j - 1][k - 1];
        putc(charstate, outfile);
        if (k % 10 == 0 && k % 60 != 0)
          putc(' ', outfile);
      }
      putc('\n', outfile);
    }
    putc('\n', outfile);
  }  
  putc('\n', outfile);
}  /* input_protdata */


void makeweights()
{
  /* make up weights vector to avoid duplicate computations */
  long i;
     
   for (i = 1; i <= sites; i++) {
    alias[i - 1] = i;
    ally[i - 1] = 0;
    aliasweight[i - 1] = weight[i - 1];
    location[i - 1] = 0;
  }  
  sitesort2(sites, aliasweight);
  sitecombine2(sites, aliasweight);
  sitescrunch2(sites, 1, 2, aliasweight);
  for (i = 1; i <= sites; i++) {
    if (aliasweight[i - 1] > 0)
      endsite = i;
  }  
  for (i = 1; i <= endsite; i++) {
    ally[alias[i - 1] - 1] = alias[i - 1];
    location[alias[i - 1] - 1] = i;
  }  
  mp = (vall *) Malloc(sites*sizeof(vall));
  contribution = (contribarr *) Malloc( endsite*sizeof(contribarr));
}  /* makeweights */


void prot_makevalues(long categs, pointarray treenode, long endsite,
                        long spp, sequence y, steptr alias)
{   
  /* set up fractional likelihoods at tips   */
  /* a version of makevalues2 found in seq.c */
  /* used by proml                           */
  long i, j, k, l;
  long b;
    
  for (k = 0; k < endsite; k++) {
    j = alias[k];
    for (i = 0; i < spp; i++) {
      for (l = 0; l < categs; l++) {
        memset(treenode[i]->protx[k][l], 0, sizeof(double)*20);
        switch (y[i][j - 1]) {
    
        case 'A':
          treenode[i]->protx[k][l][0] = 1.0;
          break;
    
        case 'R':
          treenode[i]->protx[k][l][(long)arginine   - (long)alanine] = 1.0;
          break;
    
        case 'N':
          treenode[i]->protx[k][l][(long)asparagine - (long)alanine] = 1.0;
          break;
    
        case 'D':
          treenode[i]->protx[k][l][(long)aspartic   - (long)alanine] = 1.0;
          break;
    
        case 'C':
          treenode[i]->protx[k][l][(long)cysteine   - (long)alanine] = 1.0;
          break;
     
        case 'Q':
          treenode[i]->protx[k][l][(long)glutamine  - (long)alanine] = 1.0;
          break;
    
        case 'E':
          treenode[i]->protx[k][l][(long)glutamic   - (long)alanine] = 1.0;
          break;
    
        case 'G':
          treenode[i]->protx[k][l][(long)glycine    - (long)alanine] = 1.0;
          break;
    
        case 'H':
          treenode[i]->protx[k][l][(long)histidine  - (long)alanine] = 1.0;
          break;
    
        case 'I':
          treenode[i]->protx[k][l][(long)isoleucine - (long)alanine] = 1.0;
          break;
    
        case 'L':
          treenode[i]->protx[k][l][(long)leucine    - (long)alanine] = 1.0;
          break;
    
        case 'K':
          treenode[i]->protx[k][l][(long)lysine     - (long)alanine] = 1.0;
          break;
    
        case 'M':
          treenode[i]->protx[k][l][(long)methionine - (long)alanine] = 1.0;
          break;
    
        case 'F':
          treenode[i]->protx[k][l][(long)phenylalanine - (long)alanine] = 1.0;
          break;
    
        case 'P':
          treenode[i]->protx[k][l][(long)proline    - (long)alanine] = 1.0;
          break;
    
        case 'S':
          treenode[i]->protx[k][l][(long)serine     - (long)alanine] = 1.0;
          break;
    
        case 'T':
          treenode[i]->protx[k][l][(long)threonine  - (long)alanine] = 1.0;
          break;
    
        case 'W':
          treenode[i]->protx[k][l][(long)tryptophan - (long)alanine] = 1.0;
          break;
    
        case 'Y':
          treenode[i]->protx[k][l][(long)tyrosine   - (long)alanine] = 1.0;
          break;
    
        case 'V':
          treenode[i]->protx[k][l][(long)valine     - (long)alanine] = 1.0;
          break;
    
        case 'B':
          treenode[i]->protx[k][l][(long)asparagine - (long)alanine] = 1.0;
          treenode[i]->protx[k][l][(long)aspartic   - (long)alanine] = 1.0;
          break;
    
        case 'Z':
          treenode[i]->protx[k][l][(long)glutamine  - (long)alanine] = 1.0;
          treenode[i]->protx[k][l][(long)glutamic   - (long)alanine] = 1.0;
          break;
    
        case 'X':               /* unknown aa                       */
          for (b = 0; b <= 19; b++)
            treenode[i]->protx[k][l][b] = 1.0;
          break;
    
        case '?':               /* unknown aa                       */
          for (b = 0; b <= 19; b++)
            treenode[i]->protx[k][l][b] = 1.0;
          break;
    
        case '*':               /* stop codon symbol                */
          for (b = 0; b <= 19; b++)
            treenode[i]->protx[k][l][b] = 1.0;
          break;
    
        case '-':               /* deletion event-absent data or aa */
          for (b = 0; b <= 19; b++)
            treenode[i]->protx[k][l][b] = 1.0;
          break;
        }
      }
    }
  } 
}  /* prot_makevalues */


void getinput()
{
  long grcategs;
 
  /* reads the input data */
  if (!justwts || firstset)
    inputoptions();
  if (!justwts || firstset)
    input_protdata(seqsets[ith-1], sites);
  makeweights();
  setuptree2(curtree);
  if (!usertree) {
    setuptree2(bestree);
    if (njumble > 1)
      setuptree2(bestree2);
  } 
  grcategs = (categs > rcategs) ? categs : rcategs;
  prot_allocx(nonodes, grcategs, curtree.nodep, usertree);
  if (!usertree) {
    prot_allocx(nonodes, grcategs, bestree.nodep, 0);
    if (njumble > 1)
      prot_allocx(nonodes, grcategs, bestree2.nodep, 0);
  } 
  prot_makevalues(rcategs, curtree.nodep, endsite, spp, y, alias);
}  /* getinput */


void prot_inittable()
{
  /* Define a lookup table. Precompute values and print them out in tables */
  /* Allocate memory for the pmatrices, dpmatices and ddpmatrices          */
  long i, j, k, l;
  double sumrates;

  /* Allocate memory for pmatrices, the array of pointers to pmatrices     */

  pmatrices = (double *****) Malloc (spp * sizeof(double ****));

  /* Allocate memory for the first 2 pmatrices, the matrix of conversion   */
  /* probabilities, but only once per run (aka not on the second jumble.   */

    alloc_pmatrix(0);
    alloc_pmatrix(1);

  /*  Allocate memory for one dpmatrix, the first derivative matrix        */

  dpmatrix = (double ****) Malloc( rcategs * sizeof(double ***));
  for (j = 0; j < rcategs; j++) {
    dpmatrix[j] = (double ***) Malloc( categs * sizeof(double **));
    for (k = 0; k < categs; k++) {
      dpmatrix[j][k] = (double **) Malloc( 20 * sizeof(double *));
      for (l = 0; l < 20; l++)
        dpmatrix[j][k][l] = (double *) Malloc( 20 * sizeof(double));
    }
  }

  /*  Allocate memory for one ddpmatrix, the second derivative matrix      */
  ddpmatrix = (double ****) Malloc( rcategs * sizeof(double ***));
  for (j = 0; j < rcategs; j++) {
    ddpmatrix[j] = (double ***) Malloc( categs * sizeof(double **));
    for (k = 0; k < categs; k++) {
      ddpmatrix[j][k] = (double **) Malloc( 20 * sizeof(double *));
      for (l = 0; l < 20; l++)
        ddpmatrix[j][k][l] = (double *) Malloc( 20 * sizeof(double));
    }
  }

  /* Allocate memory and assign values to tbl, the matrix of possible rates*/

  tbl = (double **) Malloc( rcategs * sizeof(double *));
  for (j = 0; j < rcategs; j++)
    tbl[j] = (double *) Malloc( categs * sizeof(double));

  for (j = 0; j < rcategs; j++)
    for (k = 0; k < categs; k++)
      tbl[j][k] = rrate[j]*rate[k];

  sumrates = 0.0;
  for (i = 0; i < endsite; i++) {
    for (j = 0; j < rcategs; j++)
      sumrates += aliasweight[i] * probcat[j]
        * tbl[j][category[alias[i] - 1] - 1];
  }
  sumrates /= (double)sites;
  for (j = 0; j < rcategs; j++)
    for (k = 0; k < categs; k++) {
      tbl[j][k] /= sumrates;
    }
  if (gama || invar) {
    fprintf(outfile, "\nDiscrete approximation to gamma distributed rates\n");
    fprintf(outfile,
    " Coefficient of variation of rates = %f  (alpha = %f)\n", cv, alpha);
  }
  if (rcategs > 1) {
    fprintf(outfile, "\nState in HMM    Rate of change    Probability\n\n");
    for (i = 0; i < rcategs; i++)
      if (probcat[i] < 0.0001)
        fprintf(outfile, "%9ld%16.3f%20.6f\n", i+1, rrate[i], probcat[i]);
      else if (probcat[i] < 0.001)
          fprintf(outfile, "%9ld%16.3f%19.5f\n", i+1, rrate[i], probcat[i]);
        else if (probcat[i] < 0.01)
            fprintf(outfile, "%9ld%16.3f%18.4f\n", i+1, rrate[i], probcat[i]);
          else
            fprintf(outfile, "%9ld%16.3f%17.3f\n", i+1, rrate[i], probcat[i]);
    putc('\n', outfile);
    if (auto_) {
      fprintf(outfile,
     "Expected length of a patch of sites having the same rate = %8.3f\n",
             1/lambda);
      putc('\n', outfile);
    }
  }
  if (categs > 1) {
    fprintf(outfile, "\nSite category   Rate of change\n\n");
    for (k = 0; k < categs; k++)
      fprintf(outfile, "%9ld%16.3f\n", k+1, rate[k]);
    fprintf(outfile, "\n\n");
  }
}  /* prot_inittable */


void alloc_pmatrix(long sib)
{
  /* Allocate memory for a new pmatrix.  Called iff num_sibs>max_num_sibs */
  long j, k, l;
  double ****temp_matrix;

  temp_matrix = (double ****) Malloc (rcategs * sizeof(double ***));
  for (j = 0; j < rcategs; j++) {
    temp_matrix[j] = (double ***) Malloc(categs * sizeof(double **));
    for (k = 0; k < categs; k++) {
      temp_matrix[j][k] = (double **) Malloc(20 * sizeof (double *));
      for (l = 0; l < 20; l++)
        temp_matrix[j][k][l] = (double *) Malloc(20 * sizeof(double));
    }
  }  
  pmatrices[sib] = temp_matrix;
  max_num_sibs++;
}  /* alloc_pmatrix */


void make_pmatrix(double **matrix, double **dmat, double **ddmat,
                        long derivative, double lz, double rat,
                        double *eigmat, double **probmat)
{
  /* Computes the R matrix such that matrix[m][l] is the joint probability */
  /* of m and l.                                                           */
  /* Computes a P matrix such that matrix[m][l] is the conditional         */
  /* probability of m given l.  This is accomplished by dividing all terms */
  /* in the R matrix by freqaa[m], the frequency of l.                     */

  long k, l, m;                 /* (l) original character state */
                                /* (m) final    character state */
                                /* (k) lambda counter           */
  double p0, p1, p2, q;
  double elambdat[20], delambdat[20], ddelambdat[20];
                                /* exponential term for matrix  */
                                /* and both derivative matrices */

  for (k = 0; k <= 19; k++) {
    elambdat[k] = exp(lz * rat * eigmat[k]);
    if(derivative != 0) {
        delambdat[k] = (elambdat[k] * rat * eigmat[k]);
        ddelambdat[k] = (delambdat[k] * rat * eigmat[k]);
    }
   } 
  for (m = 0; m <= 19; m++) {
    for (l = 0; l <= 19; l++) {
      p0 = 0.0;
      p1 = 0.0;
      p2 = 0.0;
      for (k = 0; k <= 19; k++) {
        q = probmat[k][m] * probmat[k][l];
        p0 += (q * elambdat[k]);
        if(derivative !=0) {
          p1 += (q * delambdat[k]);
          p2 += (q * ddelambdat[k]);
        }
      }
      matrix[m][l] = p0 / freqaa[m];
      if(derivative != 0) {
        dmat[m][l] = p1 / freqaa[m];
        ddmat[m][l] = p2 / freqaa[m];
      }
    }
  }  
}  /* make_pmatrix */


void prot_nuview(node *p)
{
  long b, i, j, k, l, m, num_sibs, sib_index;
  node *sib_ptr, *sib_back_ptr;
  psitelike prot_xx, x2;
  double lw, prod7;
  double **pmat;

  /* Figure out how many siblings the current node has  */
  /* and be sure that pmatrices is large enough         */
  num_sibs = count_sibs(p);
  for (i = 0; i < num_sibs; i++)
    if (pmatrices[i] == NULL)
      alloc_pmatrix(i);
 
  /* Recursive calls, should be called for all children */
  sib_ptr = p;
  for (i=0 ; i < num_sibs; i++) {
    sib_ptr      = sib_ptr->next;
    sib_back_ptr = sib_ptr->back;
 
    if (!(sib_back_ptr == NULL)) 
      if (!sib_back_ptr->tip && !sib_back_ptr->initialized)
        prot_nuview(sib_back_ptr);
  }       

  /* Make pmatrices for all possible combinations of category, rcateg      */
  /* and sib                                                               */
  sib_ptr = p;                          /* return to p */
  for (sib_index=0; sib_index < num_sibs; sib_index++) {
    sib_ptr      = sib_ptr->next;
    sib_back_ptr = sib_ptr->back;

    if (sib_back_ptr != NULL)
      lw =  fabs(p->tyme - sib_back_ptr->tyme);
    else
      lw = 0.0;

    for (j = 0; j < rcategs; j++)
      for (k = 0; k < categs; k++)
        make_pmatrix(pmatrices[sib_index][j][k], NULL, NULL, 0, lw,
                                        tbl[j][k], eigmat, probmat);
  }            
               
  for (i = 0; i < endsite; i++) {
    k = category[alias[i]-1] - 1;
    for (j = 0; j < rcategs; j++) {
          
      /* initialize to 1 all values of prot_xx */
      for (m = 0; m <= 19; m++)
        prot_xx[m] = 1;
          
      sib_ptr = p;                      /* return to p */
      /* loop through all sibs and calculate likelihoods for all possible*/
      /* amino acid combinations                                         */
      for (sib_index=0; sib_index < num_sibs; sib_index++) {
        sib_ptr      = sib_ptr->next;
        sib_back_ptr = sib_ptr->back;
          
        if (sib_back_ptr != NULL)
          memcpy(x2, sib_back_ptr->protx[i][j], sizeof(psitelike));
        else 
          for (b = 0; b <= 19; b++)
            x2[b] = 1.0;
        pmat = pmatrices[sib_index][j][k];
        for (m = 0; m <= 19; m++) {
          prod7 = 0;
          for (l = 0; l <= 19; l++)
            prod7 += (pmat[m][l] * x2[l]);
          prot_xx[m] *= prod7;
        }  
      }    
      /* And the final point of this whole function: */
      memcpy(p->protx[i][j], prot_xx, sizeof(psitelike));
    }      
  }        
  
  p->initialized = true;
}  /* prot_nuview */


void getthree(node *p, double thigh, double tlow)
{
  /* compute likelihood at a new triple of points */
  int i;
  double tt = p->tyme;
  double td = fabs(tdelta);

  x[0] = tt - td;
  x[1] = tt;
  x[2] = tt + td;

  if ( x[0] < tlow + epsilon ) {
    x[0] = tlow + epsilon;
    x[1] = ( x[0] + x[2] ) / 2;
  }

  if ( x[2] > thigh - epsilon ) {
    x[2] = thigh - epsilon;
    x[1] = ( x[0] + x[2] ) / 2;
  }
  
  for ( i = 0 ; i < 3 ; i++ ) {
    p->tyme = x[i];
    prot_nuview(p);
    lnl[i] = prot_evaluate(p);
  }
}  /* getthree */

void makenewv(node *p)
{
  /* improve a node time */
  long it, imin, imax, i, num_sibs;
  double tt, tfactor, tlow, thigh, oldlike, ymin, ymax, s32, s21, yold;
  boolean done, already;
  node *s, *sdown, *sib_ptr, *sib_back_ptr;

  s = curtree.nodep[p->index - 1];
  sdown = s->back;
  if (s == curtree.root)
    tlow = -10.0;
  else
    tlow = sdown->tyme;

  sib_ptr = s;
  num_sibs = count_sibs(p);

  thigh = s->next->back->tyme;
  for (i=0 ; i < num_sibs; i++) {
    sib_ptr      = sib_ptr->next;
    sib_back_ptr = sib_ptr->back;
      if (sib_back_ptr->tyme < thigh)
        thigh = sib_back_ptr->tyme;
  }  
  done = (thigh - tlow < 4.0*epsilon);
  it = 1;
  if (s != curtree.root)
    tdelta = (thigh - tlow) / 10.0;
  else
    tdelta = (thigh - s->tyme) / 5.0;
  tfactor = 1.0;
  if (!done)
    getthree(s, thigh, tlow);
  while (it < iterations && !done) {
    tt = s->tyme;
    ymax = lnl[0];
    imax = 1;
    for (i = 2; i <= 3; i++) {
      if (lnl[i - 1] > ymax) {
        ymax = lnl[i - 1];
        imax = i;
      } 
    }  
    if (imax != 2) {
      ymax = x[1];
      x[1] = x[imax - 1];
      x[imax - 1] = ymax;
      ymax = lnl[1];
      lnl[1] = lnl[imax - 1];
      lnl[imax - 1] = ymax;
    }
    tt = x[1];
    oldlike = lnl[1];
    yold = tt;
    s32 = (lnl[2] - lnl[1]) / (x[2] - x[1]);
    s21 = (lnl[1] - lnl[0]) / (x[1] - x[0]);
    if (fabs(x[2] - x[0]) > epsilon)
      curv = (s32 - s21) / ((x[2] - x[0]) / 2);
    else
      curv = 0.0;
    slope = (s32 + s21) / 2 - curv * (x[2] - 2 * x[1] + x[0]) / 4;
    if (curv >= 0.0) {
      if (slope < 0)
        tdelta = -fabs(tdelta);
      else
        tdelta = fabs(tdelta);
    } else
      tdelta = -(tfactor * slope / curv);
    if (tt + tdelta <= tlow + epsilon)
      tdelta = tlow + epsilon - tt;
    if (tt + tdelta >= thigh - epsilon)
      tdelta = thigh - epsilon - tt;
    tt += tdelta;
    done = (fabs(yold - tt) < epsilon || fabs(tdelta) < epsilon);
    s->tyme = tt;
    prot_nuview(s);
    lnlike = prot_evaluate(s);
    ymin = lnl[0];
    imin = 1;
    for (i = 2; i <= 3; i++) {
      if (lnl[i - 1] < ymin) {
        ymin = lnl[i - 1];
        imin = i;
      }
    }
    already = (tt == x[0]) || (tt == x[1]) || (tt == x[2]);
    if (!already && ymin < lnlike) {
      x[imin - 1] = tt;
      lnl[imin - 1] = lnlike;
    }
    if (already || lnlike < oldlike) {
      tt = x[1];
      tfactor /= 2;
      tdelta /= 2;
      curtree.likelihood = oldlike;
      lnlike = oldlike;
    } else
      tfactor = 1.0;
    
    if (!done) {
      sib_ptr = p;
      num_sibs = count_sibs(p);
      p->tyme = tt;
      for (i=0 ; i < num_sibs; i++) {
        sib_ptr      = sib_ptr->next;
        sib_ptr->tyme = tt;
      }
      
      sib_ptr = p;
      prot_nuview(p);
      for (i=0 ; i < num_sibs; i++) {
        sib_ptr      = sib_ptr->next;
        prot_nuview(sib_ptr);
      }
    }
    
    it++;
  } 
  sib_ptr = p;
  for (i=0 ; i < num_sibs; i++) {
    sib_ptr      = sib_ptr->next;
    inittrav (sib_ptr);
  } 
  smoothed = smoothed && done;
}  /* makenewv */


void update(node *p)
{
  node *sib_ptr, *sib_back_ptr;
  long i, num_sibs;
 
  /* improve time and recompute views at a node */
  if (p == NULL)
    return;
  if (p->back != NULL) {
    if (!p->back->tip && !p->back->initialized)
      prot_nuview(p->back);
  }
   
  sib_ptr = p;
  num_sibs = count_sibs(p);
  for (i=0 ; i < num_sibs; i++) {
    sib_ptr      = sib_ptr->next;
    sib_back_ptr = sib_ptr->back;
    if (sib_back_ptr != NULL) {
      if (!sib_back_ptr->tip && !sib_back_ptr->initialized)
        prot_nuview(sib_back_ptr);
    }
  }  
   
  if ((!usertree) || (usertree && !lngths) || p->iter) {
    makenewv(p);
    return;
  } 
  prot_nuview(p);
    
  sib_ptr = p;
  num_sibs = count_sibs(p);
  for (i=0 ; i < num_sibs; i++) {
    sib_ptr      = sib_ptr->next;
    prot_nuview(sib_ptr);
  } 
}  /* update */


void smooth(node *p)
{    
  node *sib_ptr;
  long i, num_sibs;
     
  if (p == NULL)
    return;
  if (p->tip)
    return;
     
  update(p);
     
  smoothed = false;
  sib_ptr = p;
  num_sibs = count_sibs(p);
  for (i=0; i < num_sibs; i++) {
    sib_ptr = sib_ptr->next;
    if (polishing || (smoothit && !smoothed)) {
      smooth(sib_ptr->back);
      p->initialized = false;
      sib_ptr->initialized = false;
    }
    update(p);
  }  
}  /* smooth */


void promlk_add(node *below, node *newtip, node *newfork, boolean tempadd)
{
  /* inserts the nodes newfork and its descendant, newtip, into the tree. */
  long i;
  boolean done;
  node *p;

  below = curtree.nodep[below->index - 1];
  newfork = curtree.nodep[newfork->index-1];
  newtip = curtree.nodep[newtip->index-1];
  if (below->back != NULL)
    below->back->back = newfork;
  newfork->back = below->back;
  below->back = newfork->next->next;
  newfork->next->next->back = below;
  newfork->next->back = newtip;
  newtip->back = newfork->next;
  if (newtip->tyme < below->tyme)
    p = newtip;
  else p = below;
  newfork->tyme = p->tyme;
  if (curtree.root == below)
    curtree.root = newfork;
  if (newfork->back != NULL) {
    if (p->tyme > newfork->back->tyme)
      newfork->tyme = (p->tyme + newfork->back->tyme) / 2.0;
    else newfork->tyme = p->tyme - epsilon;
    newfork->next->tyme = newfork->tyme;
    newfork->next->next->tyme = newfork->tyme;
    do {
      p = curtree.nodep[p->back->index - 1];
      done = (p == curtree.root);
      if (!done)
        done = (curtree.nodep[p->back->index - 1]->tyme < p->tyme - epsilon);
      if (!done) {
        curtree.nodep[p->back->index - 1]->tyme = p->tyme - epsilon;
        curtree.nodep[p->back->index - 1]->next->tyme = p->tyme - epsilon;
        curtree.nodep[p->back->index - 1]->next->next->tyme = p->tyme - epsilon;
      }
    } while (!done);
  } else {
      newfork->tyme = newfork->tyme - 2*epsilon;
      newfork->next->tyme = newfork->tyme;
      newfork->next->next->tyme = newfork->tyme;
    }
  inittrav(newtip);
  inittrav(newtip->back);
  smoothed = false;
  i = 1;
  while (i < smoothings && !smoothed) {
    smoothed = true;
    smooth(newfork);
    smooth(newfork->back);
    i++;
  }
}  /* promlk_add */


void promlk_re_move(node **item, node **fork, boolean tempadd)
{
  /* removes nodes item and its ancestor, fork, from the tree.
    the new descendant of fork's ancestor is made to be
    fork's second descendant (other than item).  Also
    returns pointers to the deleted nodes, item and fork */
  node *p, *q;
  long i;

  if ((*item)->back == NULL) {
    *fork = NULL;
    return;
  }  
  *item = curtree.nodep[(*item)->index-1];
  *fork = curtree.nodep[(*item)->back->index - 1];
  if (curtree.root == *fork) {
    if (*item == (*fork)->next->back)
      curtree.root = (*fork)->next->next->back;
    else
      curtree.root = (*fork)->next->back;
  }  
  p = (*item)->back->next->back;
  q = (*item)->back->next->next->back;
/* debug replace by hookup calls?  Does that have NULL protection? */
  if (p != NULL)
    p->back = q;
  if (q != NULL)
    q->back = p;
  (*fork)->back = NULL;
  p = (*fork)->next;
  while (p != *fork) {
    p->back = NULL;
    p = p->next;
  }  
  (*item)->back = NULL;
  inittrav(p);
  inittrav(q);
  if (tempadd)
    return;
  i = 1;
  while (i <= smoothings) {
    smooth(q);
    if (smoothit)
      smooth(q->back);
    i++;
  }  
}  /* promlk_re_move */


double prot_evaluate(node *p)
{
  contribarr tterm;
  static contribarr like, nulike, clai;
  double sum, sum2, sumc=0, y, prod4, prodl, frexm, sumterm, lterm;
  double **pmat;
  long i, j, k, l, m, lai;
  node *q, *r;
  psitelike x1, x2;

  sum = 0.0;

  if (p == curtree.root && (count_sibs(p) == 2)) {
    r = p->next->back;
    q = p->next->next->back;
    y = r->tyme + q->tyme - 2 * p->tyme;
    if (!r->tip && !r->initialized) prot_nuview (r);
    if (!q->tip && !q->initialized) prot_nuview (q);
  } else if (p == curtree.root) {
    /* the next two lines copy tyme and x to p->next.  Normally they are
       not initialized for an internal node. */
    /* assumes bifurcation */
    p->next->tyme = p->tyme;
    prot_nuview(p->next);
    r = p->next;
    q = p->next->back;
    y = fabs(p->next->tyme - q->tyme);
  } else {
    r = p;
    q = p->back;
    if (!r->tip && !r->initialized) prot_nuview (r);
    if (!q->tip && !q->initialized) prot_nuview (q);
    y = fabs(r->tyme - q->tyme);
  }

  for (j = 0; j < rcategs; j++)
    for (k = 0; k < categs; k++)
      make_pmatrix(pmatrices[0][j][k],NULL,NULL,0,y,tbl[j][k],eigmat,probmat);
  for (i = 0; i < endsite; i++) {
    k = category[alias[i]-1] - 1;
    for (j = 0; j < rcategs; j++) {
      memcpy(x1, r->protx[i][j], sizeof(psitelike));
      memcpy(x2, q->protx[i][j], sizeof(psitelike));
      prod4 = 0.0;
      pmat = pmatrices[0][j][k];
      for (m = 0; m <= 19; m++) {
        prodl = 0.0;
        for (l = 0; l <= 19; l++)
          prodl += (pmat[m][l] * x2[l]);
        frexm = x1[m] * freqaa[m];
        prod4 += (prodl * frexm);
      }     
      tterm[j] = prod4;
    }       
    sumterm = 0.0;
    for (j = 0; j < rcategs; j++)
      sumterm += probcat[j] * tterm[j];
    if (sumterm < 0.0)
        sumterm = 0.00000001;   /* ??? */
    lterm = log(sumterm);
    for (j = 0; j < rcategs; j++)
      clai[j] = tterm[j] / sumterm;
    memcpy(contribution[i], clai, rcategs * sizeof(double));
    if (!auto_ && usertree)
      l0gf[which - 1][i] = lterm;
    sum += aliasweight[i] * lterm;
  }    
  if (auto_) {
    for (j = 0; j < rcategs; j++)
      like[j] = 1.0;
    for (i = 0; i < sites; i++) {
      if ((ally[i] > 0) && (location[ally[i]-1] > 0)) {
        sumc = 0.0;
        for (k = 0; k < rcategs; k++)
          sumc += probcat[k] * like[k];
        sumc *= lambda;
        lai = location[ally[i] - 1];
        memcpy(clai, contribution[lai - 1], rcategs*sizeof(double));
        for (j = 0; j < rcategs; j++)
          nulike[j] = ((1.0 - lambda) * like[j] + sumc) * clai[j];
      } else {
        for (j = 0; j < rcategs; j++)
          nulike[j] = ((1.0 - lambda) * like[j] + sumc);
      }
      memcpy(like, nulike, rcategs * sizeof(double));
    }  
    sum2 = 0.0;
    for (i = 0; i < rcategs; i++)
      sum2 += probcat[i] * like[i];
    sum += log(sum2);
  }    
  curtree.likelihood = sum;
  if (auto_ || !usertree)
    return sum;
  l0gl[which - 1] = sum;
  if (which == 1) {
    maxwhich = 1;
    maxlogl = sum;
    return sum;
  }    
  if (sum > maxlogl) {
    maxwhich = which;
    maxlogl = sum;
  }    
  return sum;
}  /* prot_evaluate */


void tryadd(node *p, node **item, node **nufork)
{  /* temporarily adds one fork and one tip to the tree.
    if the location where they are added yields greater
    likelihood than other locations tested up to that
    time, then keeps that location as there */
     
  long grcategs;
  grcategs = (categs > rcategs) ? categs : rcategs;
     
  promlk_add(p, *item, *nufork, true);
  like = prot_evaluate(p);
  if (lastsp) {
      if (like >= bestyet || bestyet == UNDEFINED)
            prot_copy_(&curtree, &bestree, nonodes, grcategs);
  }  
  if (like > bestyet || bestyet == UNDEFINED) {
    bestyet = like;
    there = p;
  }  
  promlk_re_move(item, nufork, true);
}  /* tryadd */


void addpreorder(node *p, node *item_, node *nufork_, boolean contin,
                        boolean continagain)
{      
  /* traverses a binary tree, calling function tryadd
    at a node before calling tryadd at its descendants */
  node *item, *nufork;
     
  item = item_;
  nufork = nufork_;
  if (p == NULL)
    return;
  tryadd(p, &item, &nufork);
  contin = continagain;
  if ((!p->tip) && contin) {
    addpreorder(p->next->back, item, nufork, contin, continagain);
    addpreorder(p->next->next->back, item, nufork, contin, continagain);
  }  
}  /* addpreorder */


void restoradd(node *below, node *newtip, node *newfork, double prevtyme)
{
/* restore "new" tip and fork to place "below".  restore tymes */
/* assumes bifurcation */
  hookup(newfork, below->back);
  hookup(newfork->next, below);
  hookup(newtip, newfork->next->next);
  curtree.nodep[newfork->index-1] = newfork;
  newfork->tyme = prevtyme;
/* assumes bifurcations */
  newfork->next->tyme = prevtyme;
  newfork->next->next->tyme = prevtyme;
} /* restoradd */


void tryrearr(node *p, boolean *success)
{
  /* evaluates one rearrangement of the tree.
    if the new tree has greater likelihood than the old
    one sets success = TRUE and keeps the new tree.
    otherwise, restores the old tree */
  node *frombelow, *whereto, *forknode;
  double oldlike, prevtyme;
  boolean wasonleft;

  if (p == curtree.root)
    return;
  forknode = curtree.nodep[p->back->index - 1];
  if (forknode == curtree.root)
    return;
  oldlike = bestyet;
  prevtyme = forknode->tyme;
/* the following statement presumes bifurcating tree */
  if (forknode->next->back == p) {
    frombelow = forknode->next->next->back;
    wasonleft = true;
  }
  else {
    frombelow = forknode->next->back;
    wasonleft = false;
  }
  whereto = curtree.nodep[forknode->back->index - 1];
  promlk_re_move(&p, &forknode, true);
  promlk_add(whereto, p, forknode, true);
  like = prot_evaluate(p);
  if (like <= oldlike && oldlike != UNDEFINED) {
    promlk_re_move(&p, &forknode, true);
    restoradd(frombelow, p, forknode, prevtyme);
    if (wasonleft && (forknode->next->next->back == p)) {
       hookup (forknode->next->back, forknode->next->next);
       hookup (forknode->next, p);
    }
    curtree.likelihood = oldlike;
    inittrav(forknode);
    inittrav(forknode->next);
    inittrav(forknode->next->next);
  } else {
    (*success) = true;
    bestyet = like;
  }
}  /* tryrearr */


void repreorder(node *p, boolean *success)
{    
  /* traverses a binary tree, calling function tryrearr
    at a node before calling tryrearr at its descendants */
  if (p == NULL)
    return;
  tryrearr(p, success);
  if (p->tip)
    return;
  if (!(*success))
    repreorder(p->next->back, success);
  if (!(*success))
    repreorder(p->next->next->back, success);
}  /* repreorder */


void rearrange(node **r)
{    
  /* traverses the tree (preorder), finding any local
    rearrangement which increases the likelihood.
    if traversal succeeds in increasing the tree's
    likelihood, function rearrange runs traversal again */
  boolean success;
  success = true;
  while (success) {
    success = false;
    repreorder(*r, &success);
  } 
}  /* rearrange */


void nodeinit(node *p)
{
  /* set up times at one node */
  node *sib_ptr, *sib_back_ptr;
  long i, num_sibs;
  double lowertyme;

  sib_ptr = p;
  num_sibs = count_sibs(p);

  /* lowertyme = lowest of children's times */
  lowertyme = p->next->back->tyme;
  for (i=0 ; i < num_sibs; i++) {
    sib_ptr      = sib_ptr->next;
    sib_back_ptr = sib_ptr->back;
    if (sib_back_ptr->tyme < lowertyme)
      lowertyme = sib_back_ptr->tyme;
  }  
   
  p->tyme = lowertyme - 0.1;

  sib_ptr = p;
  for (i=0 ; i < num_sibs; i++) {
    sib_ptr = sib_ptr->next;
    sib_back_ptr = sib_ptr->back;

    sib_ptr->tyme = p->tyme;
    sib_back_ptr->v = sib_back_ptr->tyme - p->tyme;
    sib_ptr->v = sib_back_ptr->v;
  }  
}  /* nodeinit */


void initrav(node *p)
{

  long i, num_sibs;
  node *sib_ptr, *sib_back_ptr;

  /* traverse to set up times throughout tree */
  if (p->tip)
    return;

  sib_ptr = p;
  num_sibs = count_sibs(p);
  for (i=0 ; i < num_sibs; i++) {
    sib_ptr      = sib_ptr->next;
    sib_back_ptr = sib_ptr->back;
    initrav(sib_back_ptr);
  }

  nodeinit(p);
}  /* initrav */


void travinit(node *p)
{
  long i, num_sibs;
  node *sib_ptr, *sib_back_ptr;

  /* traverse to set up initial values */
  if (p == NULL)
    return;
  if (p->tip)
    return;
  if (p->initialized)
    return;


  sib_ptr = p;
  num_sibs = count_sibs(p);
  for (i=0 ; i < num_sibs; i++) {
    sib_ptr      = sib_ptr->next;
    sib_back_ptr = sib_ptr->back;
    travinit(sib_back_ptr);
  }

  prot_nuview(p);
  p->initialized = true;
}  /* travinit */


void travsp(node *p)
{
  long i, num_sibs;
  node *sib_ptr, *sib_back_ptr;
    
  /* traverse to find tips */
  if (p == curtree.root)
    travinit(p);
  if (p->tip)
    travinit(p->back);
  else {
    sib_ptr = p;
    num_sibs = count_sibs(p);
    for (i=0 ; i < num_sibs; i++) {
      sib_ptr      = sib_ptr->next;
      sib_back_ptr = sib_ptr->back;
      travsp(sib_back_ptr);
    }
  } 
}  /* travsp */


void treevaluate()
{
  /* evaluate likelihood of tree, after iterating branch lengths */
  long i, j,  num_sibs;
  node *sib_ptr, *sib_back_ptr;
  double dummy;

  polishing = true;
  smoothit = true;
  for (i = 0; i < spp; i++)
    curtree.nodep[i]->initialized = false;
  for (i = spp; i < nonodes; i++) {
    sib_ptr = curtree.nodep[i];
    sib_ptr->initialized = false;
    num_sibs = count_sibs(sib_ptr);
    for (j=0 ; j < num_sibs; j++) {
      sib_ptr      = sib_ptr->next;
      sib_back_ptr = sib_ptr->back;
      sib_ptr->initialized = false;
    }
     
  }  
  if (!lngths)
    initrav(curtree.root);
  travsp(curtree.root);
  for (i = 1; i <= smoothings * 4; i++)
    smooth(curtree.root);
  dummy = prot_evaluate(curtree.root);
}  /* treevaluate */


void promlk_coordinates(node *p, long *tipy)
{
  /* establishes coordinates of nodes */
  node *q, *first, *last, *pp1 =NULL, *pp2 =NULL;
  long num_sibs, p1, p2, i;

  if (p->tip) {
    p->xcoord = 0;
    p->ycoord = (*tipy);
    p->ymin   = (*tipy);
    p->ymax   = (*tipy);
    (*tipy)  += down;
    return;
  }
  q = p->next;
  do {
    promlk_coordinates(q->back, tipy);
    q = q->next;
  } while (p != q);
  num_sibs = count_sibs(p);
  p1 = (long)((num_sibs+1)/2.0);
  p2 = (long)((num_sibs+2)/2.0);
  i = 1;
  q = p->next;
  first  = q->back;
  do {
    if (i == p1) pp1 = q->back;
    if (i == p2) pp2 = q->back;
    last = q->back;
    q = q->next;
    i++;
  } while (q != p);
  p->xcoord = (long)(0.5 - over * p->tyme);
  p->ycoord = (pp1->ycoord + pp2->ycoord) / 2;
  p->ymin = first->ymin;
  p->ymax = last->ymax;
}  /* promlk_coordinates */


void promlk_drawline(long i, double scale)
{           
  /* draws one row of the tree diagram by moving up tree */
  node *p, *q, *r, *first =NULL, *last =NULL;
  long n, j;
  boolean extra, done;
            
  p = curtree.root;
  q = curtree.root;
  extra = false;
  if ((long)(p->ycoord) == i) {
    if (p->index - spp >= 10)
      fprintf(outfile, "-%2ld", p->index - spp);
    else    
      fprintf(outfile, "--%ld", p->index - spp);
    extra = true;
  } else    
    fprintf(outfile, "  ");
  do {      
    if (!p->tip) {
      r = p->next;
      done = false;
      do {  
        if (i >= r->back->ymin && i <= r->back->ymax) {
          q = r->back;
          done = true;
        }   
        r = r->next;
      } while (!(done || r == p));
      first = p->next->back;
      r = p->next;
      while (r->next != p)
        r = r->next;
      last = r->back;
    }       
    done = (p == q);
    n = (long)(scale * ((long)(p->xcoord) - (long)(q->xcoord)) + 0.5);
    if (n < 3 && !q->tip)
      n = 3;
    if (extra) {
      n--;  
      extra = false;
    }       
    if ((long)(q->ycoord) == i && !done) {
      if (p->ycoord != q->ycoord)
        putc('+', outfile);
      else  
        putc('-', outfile);
      if (!q->tip) {
        for (j = 1; j <= n - 2; j++)
          putc('-', outfile);
        if (q->index - spp >= 10)
          fprintf(outfile, "%2ld", q->index - spp);
        else
          fprintf(outfile, "-%ld", q->index - spp);
        extra = true;
      } else {
        for (j = 1; j < n; j++)
          putc('-', outfile);
      }     
    } else if (!p->tip) {
      if ((long)(last->ycoord) > i && (long)(first->ycoord) < i &&
           i != (long)(p->ycoord)) {
        putc('!', outfile);
        for (j = 1; j < n; j++)
          putc(' ', outfile);
      } else {
        for (j = 1; j <= n; j++)
          putc(' ', outfile);
      }     
    } else {
      for (j = 1; j <= n; j++)
        putc(' ', outfile);
    }       
    if (p != q)
      p = q;
  } while (!done);
  if ((long)(p->ycoord) == i && p->tip) {
    for (j = 0; j < nmlngth; j++)
      putc(nayme[p->index - 1][j], outfile);
  }         
  putc('\n', outfile);
}  /* promlk_drawline */
            

void promlk_printree()
{           
 /* prints out diagram of the tree */
  long tipy;
  double scale;
  long i;   
  node *p;  
         
  if (!treeprint)
    return; 
  putc('\n', outfile);
  tipy = 1; 
  promlk_coordinates(curtree.root, &tipy);
  p = curtree.root;
  while (!p->tip)
    p = p->next->back;
  scale = 1.0 / (long)(p->tyme - curtree.root->tyme + 1.000);
  putc('\n', outfile);
  for (i = 1; i <= tipy - down; i++)
    promlk_drawline(i, scale);
  putc('\n', outfile);
}  /* promlk_printree */


void describe(node *p)
{
  long i, num_sibs;
  node *sib_ptr, *sib_back_ptr;
  double v;

  if (p == curtree.root)
    fprintf(outfile, " root         ");
  else
    fprintf(outfile, "%4ld          ", p->back->index - spp);
  if (p->tip) {
    for (i = 0; i < nmlngth; i++)
      putc(nayme[p->index - 1][i], outfile);
  } else
    fprintf(outfile, "%4ld      ", p->index - spp);
  if (p != curtree.root) {
    fprintf(outfile, "%11.5f", (p->tyme - curtree.root->tyme));
    v = (p->tyme - curtree.nodep[p->back->index - 1]->tyme);
    fprintf(outfile, "%13.5f", v);
  }  
  putc('\n', outfile);
  if (!p->tip) {

    sib_ptr = p;
    num_sibs = count_sibs(p);
    for (i=0 ; i < num_sibs; i++) {
      sib_ptr      = sib_ptr->next;
      sib_back_ptr = sib_ptr->back;
      describe(sib_back_ptr);
    }
  }  
}  /* describe */


void prot_reconstr(node *p, long n)
{
  /* reconstruct and print out acid at site n+1 at node p */
  long i, j, k, first, num_sibs = 0;
  double f, sum, xx[20];
  node *q = NULL;

  if (p->tip)
    putc(y[p->index-1][n], outfile);
  else {
    num_sibs = count_sibs(p);
    if ((ally[n] == 0) || (location[ally[n]-1] == 0))
      putc('.', outfile);
    else {
      j = location[ally[n]-1] - 1;
      sum = 0;
      for (i = 0; i <= 19; i++) {
        f = p->protx[j][mx-1][i];
        if (!p->tip) {
          q = p;
          for (k = 0; k < num_sibs; k++) {
            q = q->next;
            f *= q->protx[j][mx-1][i];
          }
        }
        f = sqrt(f);
        xx[i] = f * freqaa[i];
        sum += xx[i];
      }
      for (i = 0; i <= 19; i++)
        xx[i] /= sum;
      first = 0;
      for (i = 0; i <= 19; i++)
        if (xx[i] > xx[first])
          first = i;
      if (xx[first] > 0.95)
        putc(aachar[first], outfile);
      else
        putc(tolower((int) aachar[first]), outfile);
      if (rctgry && rcategs > 1)
        mx = mp[n][mx - 1];
      else
        mx = 1;
    }
  }
} /* prot_reconstr */


void rectrav(node *p, long m, long n)
{
  /* print out segment of reconstructed sequence for one branch */
  long num_sibs, i;
  node *sib_ptr;

  putc(' ', outfile);
  if (p->tip) {
    for (i = 0; i < nmlngth; i++)
      putc(nayme[p->index-1][i], outfile);
  } else
    fprintf(outfile, "%4ld      ", p->index - spp);
  fprintf(outfile, "  ");
  mx = mx0;
  for (i = m; i <= n; i++) {
    if ((i % 10 == 0) && (i != m))
      putc(' ', outfile);
    prot_reconstr(p, i);                          
  }  
  putc('\n', outfile);
  if (!p->tip) {
    num_sibs = count_sibs(p);
    sib_ptr = p;
    for (i = 0; i < num_sibs; i++) {
      sib_ptr = sib_ptr->next;
      rectrav(sib_ptr->back, m, n);
    }
  }  
  mx1 = mx;
}  /* rectrav */


void summarize()
{
  long i, j, mm=0;
  double mode, sum;
  double like[maxcategs], nulike[maxcategs];
  double **marginal;
  long **mp;

  mp = (long **)Malloc(sites * sizeof(long *));
  for (i = 0; i <= sites-1; ++i)
    mp[i] = (long *)Malloc(sizeof(long)*rcategs);
  fprintf(outfile, "\nLn Likelihood = %11.5f\n\n", curtree.likelihood);
  fprintf(outfile, " Ancestor      Node      Node Height     Length\n");
  fprintf(outfile, " --------      ----      ---- ------     ------\n");
  describe(curtree.root);
  putc('\n', outfile);
  if (rctgry && rcategs > 1) {
    for (i = 0; i < rcategs; i++)
      like[i] = 1.0;
    for (i = sites - 1; i >= 0; i--) {
      sum = 0.0;
      for (j = 0; j < rcategs; j++) {
        nulike[j] = (lambda1 + lambda * probcat[j]) * like[j];
        mp[i][j] = j + 1;
        for (k = 1; k <= rcategs; k++) {
          if (k != j + 1) {
            if (lambda * probcat[k - 1] * like[k - 1] > nulike[j]) {
              nulike[j] = lambda * probcat[k - 1] * like[k - 1];
              mp[i][j] = k;
            }
          }
        }
        if ((ally[i] > 0) && (location[ally[i]-1] > 0))
          nulike[j] *= contribution[location[ally[i] - 1] - 1][j];
        sum += nulike[j];
      }
      for (j = 0; j < rcategs; j++)
        nulike[j] /= sum;
      memcpy(like, nulike, rcategs * sizeof(double));
    }
    mode = 0.0;
    mx = 1;
    for (i = 1; i <= rcategs; i++) {
      if (probcat[i - 1] * like[i - 1] > mode) {
        mx = i;
        mode = probcat[i - 1] * like[i - 1];
      }
    }
    mx0 = mx;
    fprintf(outfile,
 "Combination of categories that contributes the most to the likelihood:\n\n");
    for (i = 1; i <= nmlngth + 3; i++)
      putc(' ', outfile);
    for (i = 1; i <= sites; i++) {
      fprintf(outfile, "%ld", mx);
      if (i % 10 == 0)
        putc(' ', outfile);
      if (i % 60 == 0 && i != sites) {
        putc('\n', outfile);
        for (j = 1; j <= nmlngth + 3; j++)
          putc(' ', outfile);
      }
      mx = mp[i - 1][mx - 1];
    }
    fprintf(outfile, "\n\n");
    marginal = (double **) Malloc( sites*sizeof(double *));
    for (i = 0; i < sites; i++)
      marginal[i] = (double *) Malloc( rcategs*sizeof(double));
    for (i = 0; i < rcategs; i++)
      like[i] = 1.0;
    for (i = sites - 1; i >= 0; i--) {
      sum = 0.0;
      for (j = 0; j < rcategs; j++) {
        nulike[j] = (lambda1 + lambda * probcat[j]) * like[j];
        for (k = 1; k <= rcategs; k++) {
          if (k != j + 1)
              nulike[j] += lambda * probcat[k - 1] * like[k - 1];
        }
        if ((ally[i] > 0) && (location[ally[i]-1] > 0))
          nulike[j] *= contribution[location[ally[i] - 1] - 1][j];
        sum += nulike[j];
      }
      for (j = 0; j < rcategs; j++) {
        nulike[j] /= sum;
        marginal[i][j] = nulike[j];
      }
      memcpy(like, nulike, rcategs * sizeof(double));
    }
    for (i = 0; i < rcategs; i++)
      like[i] = 1.0;
    for (i = 0; i < sites; i++) {
      sum = 0.0;
      for (j = 0; j < rcategs; j++) {
        nulike[j] = (lambda1 + lambda * probcat[j]) * like[j];
        for (k = 1; k <= rcategs; k++) {
          if (k != j + 1)
              nulike[j] += lambda * probcat[k - 1] * like[k - 1];
        }
        marginal[i][j] *= like[j] * probcat[j];
        sum += nulike[j];
      }
      for (j = 0; j < rcategs; j++)
        nulike[j] /= sum;
      memcpy(like, nulike, rcategs * sizeof(double));
      sum = 0.0;
      for (j = 0; j < rcategs; j++)
        sum += marginal[i][j];
      for (j = 0; j < rcategs; j++)
        marginal[i][j] /= sum;
    }
    fprintf(outfile, "Most probable category at each site if > 0.95");
    fprintf(outfile, " probability (\".\" otherwise)\n\n");
    for (i = 1; i <= nmlngth + 3; i++)                                         
      putc(' ', outfile);
    for (i = 0; i < sites; i++) {
      sum = 0.0;
      for (j = 0; j < rcategs; j++)
        if (marginal[i][j] > sum) {
          sum = marginal[i][j];
          mm = j;
        }
        if (sum >= 0.95)
        fprintf(outfile, "%ld", mm+1);
      else
        putc('.', outfile);
      if ((i+1) % 60 == 0) {
        if (i != 0) {
          putc('\n', outfile);
          for (j = 1; j <= nmlngth + 3; j++)
            putc(' ', outfile);
        }
      }
      else if ((i+1) % 10 == 0)
        putc(' ', outfile);
    }
    putc('\n', outfile);
    for (i = 0; i < sites; i++)
      free(marginal[i]);
    free(marginal);
  }  
  putc('\n', outfile);
  putc('\n', outfile);
  putc('\n', outfile);
  if (hypstate) {
    fprintf(outfile, "Probable sequences at interior nodes:\n\n");
    fprintf(outfile, "  node       ");
    for (i = 0; (i < 13) && (i < ((sites + (sites-1)/10 - 39) / 2)); i++)
      putc(' ', outfile);
    fprintf(outfile, "Reconstructed sequence (caps if > 0.95)\n\n");
    if (!rctgry || (rcategs == 1))
      mx0 = 1;
    for (i = 0; i < sites; i += 60) {
      k = i + 59;
      if (k >= sites)
        k = sites - 1;
      rectrav(curtree.root, i, k);
      putc('\n', outfile);
      mx0 = mx1;
    }
  }  
}  /* summarize */


void promlk_treeout(node *p)
{
  /* write out file with representation of final tree */
  node *sib_ptr;
  long i, n, w, num_sibs;
  Char c;
  double x;

  if (p->tip) {
    n = 0;
    for (i = 1; i <= nmlngth; i++) {
      if (nayme[p->index - 1][i - 1] != ' ')
        n = i;
    }
    for (i = 0; i < n; i++) {
      c = nayme[p->index - 1][i];
      if (c == ' ')
        c = '_';
      putc(c, outtree);
    }
    col += n;
  } else {
    sib_ptr = p;
    num_sibs = count_sibs(p);
    putc('(', outtree);
    col++;

    for (i=0; i < (num_sibs - 1); i++) {
      sib_ptr = sib_ptr->next;
      promlk_treeout(sib_ptr->back);
      putc(',', outtree);
      col++;
      if (col > 55) {
        putc('\n', outtree);
        col = 0;
      }
    }
    sib_ptr = sib_ptr->next;
    promlk_treeout(sib_ptr->back);
    putc(')', outtree);
    col++;
  }  
  if (p == curtree.root) {
    fprintf(outtree, ";\n");
    return;
  }  
  x = (p->tyme - curtree.nodep[p->back->index - 1]->tyme);
  if (x > 0.0)
    w = (long)(0.4342944822 * log(x));  
  else if (x == 0.0)
    w = 0;
  else
    w = (long)(0.4342944822 * log(-x)) + 1;
  if (w < 0)
    w = 0;
  fprintf(outtree, ":%*.5f", (int)(w + 7), x);
  col += w + 8;
}  /* promlk_treeout */


void initpromlnode(node **p, node **grbg, node *q, long len, long nodei,
                        long *ntips, long *parens, initops whichinit,
                        pointarray treenode, pointarray nodep, Char *str,
                        Char *ch, char** treestr)
{
  /* initializes a node */
  boolean minusread;
  double valyew, divisor;

  switch (whichinit) {
  case bottom:
    gnu(grbg, p);
    (*p)->index = nodei;
    (*p)->tip = false;
    malloc_ppheno((*p), endsite, rcategs);
    nodep[(*p)->index - 1] = (*p);
    break;
  case nonbottom:
    gnu(grbg, p);
    malloc_ppheno(*p, endsite, rcategs);
    (*p)->index = nodei;
    break;
  case tip:
    match_names_to_data(str, nodep, p, spp);
    break;
  case iter:
    (*p)->initialized = false;
    (*p)->v = initialv;
    (*p)->iter = true;
    if ((*p)->back != NULL)
      (*p)->back->iter = true;
    break;
  case length:
    processlength(&valyew, &divisor, ch, &minusread, treestr, parens);
    (*p)->v = valyew / divisor;
    (*p)->iter = false;
    if ((*p)->back != NULL) {
      (*p)->back->v = (*p)->v;
      (*p)->back->iter = false;
    }
    break;
  case unittrwt:
    curtree.nodep[spp]->iter = false;
    break;
  default:      /* cases hslength, hsnolength, treewt */
    break;      /* should never occur                 */
  }  
} /* initpromlnode */


void tymetrav(node *p, double *x)
{
  /* set up times of nodes */
  node *sib_ptr, *q;
  long i, num_sibs;
  double xmax;

  xmax = 0.0;
  if (!p->tip) {
    sib_ptr  = p;
    num_sibs = count_sibs(p);
    for (i=0; i < num_sibs; i++) {
      sib_ptr = sib_ptr->next;
      tymetrav(sib_ptr->back, x);
      if (xmax > (*x))
        xmax = (*x);
    }
  } else
    (*x)     = 0.0;
  p->tyme  = xmax;
  if (!p->tip) {
    q = p;
    while (q->next != p) {
      q = q->next;
      q->tyme = p->tyme;
    }
  }
  (*x) = p->tyme - p->v;
}  /* tymetrav */


void free_all_protx (long nonodes, pointarray treenode)
{
  /* used in proml */
  long i, j, k;
  node *p;

  /* Zero thru spp are tips, */
  for (i = 0; i < spp; i++) {
    for (j = 0; j < endsite; j++)
      free(treenode[i]->protx[j]);
    free(treenode[i]->protx);
  }
   
  /* The rest are rings (i.e. triads) */
  for (i = spp; i < nonodes; i++) {
    if (treenode[i] != NULL) {
      p = treenode[i];
      for (j = 1; j <= 3; j++) {
        for (k = 0; k < endsite; k++)
          free(p->protx[k]);
        free(p->protx);
        p = p->next;
      } 
    }  
  }  
}  /* free_all_protx */


void maketree()
{
  /* constructs a binary tree from the pointers in curtree.nodep,
     adds each node at location which yields highest likelihood
     then rearranges the tree for greatest likelihood */

  long i, j, k, l;
  long numtrees = 0, num_sibs;
  double bestlike, gotlike, x;
  node *item, *nufork, *dummy, *q, *root=NULL;
  boolean dummy_haslengths, dummy_first, goteof;
  long nextnode;
  long grcategs;
  pointarray dummy_treenode=NULL;
  char* treestr;

  grcategs = (categs > rcategs) ? categs : rcategs;

  prot_inittable();

  if (!usertree) {
    for (i = 1; i <= spp; i++)
      enterorder[i - 1] = i;
    if (jumble)
      randumize(seed, enterorder);
    curtree.root = curtree.nodep[spp];
    curtree.root->back = NULL;
    for (i = 0; i < spp; i++)
       curtree.nodep[i]->back = NULL;
    for (i = spp; i < nonodes; i++) {
       q = curtree.nodep[i];
       q->back = NULL;
       while ((q = q->next) != curtree.nodep[i])
         q->back = NULL;
    }
    polishing = false;
    promlk_add(curtree.nodep[enterorder[0]-1], curtree.nodep[enterorder[1]-1],
                curtree.nodep[spp], false);
    if (progress) {
      printf("\nAdding species:\n");
      writename(0, 2, enterorder);
#ifdef WIN32
      phyFillScreenColor();
#endif
    } 
    lastsp = false;
    smoothit = false;
    for (i = 3; i <= spp; i++) {
      bestyet = UNDEFINED;
      bestree.likelihood = bestyet;
      there = curtree.root;
      item = curtree.nodep[enterorder[i - 1] - 1];
      nufork = curtree.nodep[spp + i - 2];
      lastsp = (i == spp);
      addpreorder(curtree.root, item, nufork, true, true);
      promlk_add(there, item, nufork, false);
      like = prot_evaluate(curtree.root);
      rearrange(&curtree.root);
      if (curtree.likelihood > bestree.likelihood) {
        prot_copy_(&curtree, &bestree, nonodes, grcategs);
      } 
      if (progress) {
        writename(i - 1, 1, enterorder);
#ifdef WIN32
        phyFillScreenColor();
#endif
      }
      if (lastsp && global) {
        if (progress) {
          printf("Doing global rearrangements\n");
          printf("  !");
          for (j = 1; j <= nonodes; j++)
            putchar('-');
          printf("!\n");
        }
        bestlike = bestyet;
        do {
          if (progress)
            printf("   ");
          gotlike = bestlike;
          for (j = 0; j < nonodes; j++) {
            bestyet = UNDEFINED;
            item = curtree.nodep[j];
            if (item != curtree.root) {
              nufork = curtree.nodep[curtree.nodep[j]->back->index - 1];
              promlk_re_move(&item, &nufork, false);
              there = curtree.root;
              addpreorder(curtree.root, item, nufork, true, true);
              promlk_add(there, item, nufork, false);
            }
            if (progress) {
              putchar('.');
              fflush(stdout);
            }
          }
          if (progress)
            putchar('\n');
        } while (bestlike < gotlike);
      }
    } 
    if (njumble > 1 && lastsp) {
      for (i = 0; i < spp; i++ )
        promlk_re_move(&curtree.nodep[i], &dummy, false);
      if (jumb == 1 || bestree2.likelihood < bestree.likelihood)
        prot_copy_(&bestree, &bestree2, nonodes, grcategs);
    } 
    if (jumb == njumble) {
      if (njumble > 1)
        prot_copy_(&bestree2, &curtree, nonodes, grcategs);
      else 
        prot_copy_(&bestree, &curtree, nonodes, grcategs);
      fprintf(outfile, "\n\n");
      treevaluate();
      curtree.likelihood = prot_evaluate(curtree.root);
      promlk_printree();
      summarize();
      if (trout) {
        col = 0;
        promlk_treeout(curtree.root);
      } 
    }  
  } else {
    /*openfile(&intree, INTREE, "input tree file", "r", progname, intreename);*/
    if (numtrees > 2)
      initseed(&inseed, &inseed0, seed);
    l0gl = (double *)Malloc(numtrees * sizeof(double));
    l0gf = (double **)Malloc(numtrees * sizeof(double *));
    for (i=0;i<numtrees;++i)
      l0gf[i] = (double *)Malloc(endsite * sizeof(double));
    if (treeprint) {
      fprintf(outfile, "User-defined tree");
      if (numtrees > 1)
        putc('s', outfile);
      fprintf(outfile, ":\n\n");
    }
    fprintf(outfile, "\n\n");
    which = 1;
    while (which <= numtrees) {

      /* These initializations required each time through the loop
         since multiple trees require re-initialization */
      dummy_haslengths = true;
      nextnode         = 0;
      dummy_first      = true;
      goteof           = false;

      treestr = ajStrStr(phylotrees[0]->Tree);
      treeread(&treestr, &root, dummy_treenode, &goteof, &dummy_first,
               curtree.nodep, &nextnode,
               &dummy_haslengths, &grbg, initpromlnode);

      nonodes = nextnode;

      root = curtree.nodep[root->index - 1];
      curtree.root = root;

      if (lngths)
        tymetrav(curtree.root, &x);

      if (goteof && (which <= numtrees)) {
        /* if we hit the end of the file prematurely */
        printf ("\n");
        printf ("ERROR: trees missing at end of file.\n");
        printf ("\tExpected number of trees:\t\t%ld\n", numtrees);
        printf ("\tNumber of trees actually in file:\t%ld.\n\n", which - 1);
        exxit(-1);
      } 
      curtree.start = curtree.nodep[0]->back;
      treevaluate();
      promlk_printree();
      summarize();
      if (trout) {
        col = 0;
        promlk_treeout(curtree.root);
      } 
      which++;
    }  
     
    FClose(intree);
    if (!auto_ && numtrees > 1 && weightsum > 1 )
      standev2(numtrees, maxwhich, 0, endsite, maxlogl, l0gl, l0gf,
               aliasweight, seed);
  }
  if (usertree) {
    free(l0gl);
    for (i=0; i < numtrees; i++)
      free(l0gf[i]);
    free(l0gf);
  }
  for (num_sibs = 0; num_sibs < max_num_sibs; num_sibs++) {
    for (j = 0; j < rcategs; j++) {
      for (k = 0; k < categs; k++) {
        for (l = 0; l < 20; l++) {
          free(pmatrices[num_sibs][j][k][l]);
        }
        free(pmatrices[num_sibs][j][k]);
      } 
     free(pmatrices[num_sibs][j]);
   }
   free(pmatrices[num_sibs]);
  }
  if (jumb < njumble)
    return;
  free(contribution);
  free(mp);
  free_all_protx(nonodes2, curtree.nodep);
  if (!usertree || reconsider) {
    free_all_protx(nonodes2, bestree.nodep);
    if (njumble > 1)
      free_all_protx(nonodes2, bestree2.nodep);
  }
  if (progress) {
    printf("\n\nOutput written to file \"%s\"\n\n", outfilename);
    if (trout)
      printf("Tree also written onto file \"%s\"\n", outtreename);
    putchar('\n');
  }

  free(root);
} /* maketree */


void clean_up()
{
  /* Free and/or close stuff */
  long i;

  free (rrate);
  free (probcat);
  free (rate);
  /* Seems to require freeing every time... */
  for (i = 0; i < spp; i++) {
    free (y[i]);
  }
  free (y);
  free (nayme);
  free (enterorder);
  free (category);
  free (weight);
  free (alias);
  free (ally);
  free (location);
  free (aliasweight);
  free (probmat);
  free (eigmat);
  if (! (njumble <= 1))
    freetree2(bestree2.nodep, nonodes2);
  FClose(infile);
  FClose(outfile);
  FClose(outtree);
#ifdef MAC
  fixmacfile(outfilename);
  fixmacfile(outtreename);
#endif
}  /* clean_up */


int main(int argc, Char *argv[])
{  /* Protein Maximum Likelihood with molecular clock */

#ifdef MAC
  argc = 1;             /* macsetup("Promlk", "");        */
  argv[0] = "Promlk";
#endif
  init(argc,argv);
  emboss_getoptions("fpromlk",argc,argv);
  progname = argv[0];
  /*openfile(&infile, INFILE, "input file", "r", argv[0], infilename);*/
  openfile(&outfile, OUTFILE, "output file", "w", argv[0], &outfilename);

  ibmpc = IBMCRT;
  ansi = ANSICRT;
  datasets = 1;
  mulsets = false;
  firstset = true;
  doinit();

  openfile(&outtree,OUTTREE,"output tree file","w",argv[0],&outtreename);
  if (!outtree) trout = false;
/*
  if (ctgry)
    openfile(&catfile,CATFILE,"categories file","r",argv[0],catfilename);
  if (weights || justwts)
   openfile(&weightfile,WEIGHTFILE,"weights file","r",argv[0],weightfilename);
*/ 
 for (ith = 1; ith <= datasets; ith++) {
    if (datasets > 1) {
      fprintf(outfile, "Data set # %ld:\n\n", ith);
      if (progress)
        printf("\nData set # %ld:\n", ith);
    }
    getinput();
    
    if (ith == 1)
      firstset = false;
    for (jumb = 1; jumb <= njumble; jumb++){
      max_num_sibs = 0;
      maketree();
    }
  }     

  clean_up();
  printf("Done.\n\n");
#ifdef WIN32
  phyRestoreConsoleAttributes();
#endif  
  return 0;
}  /* Protein Maximum Likelihood with molecular clock */

