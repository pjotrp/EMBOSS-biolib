
#include "phylip.h"
#include "seq.h"

/* version 3.6. (c) Copyright 1993-2000 by the University of Washington.
   Written by Joseph Felsenstein, Lucas Mix, Akiko Fuseki, Sean Lamont,
   Andrew Keeffe, Dan Fineman, and Patrick Colacurcio.
   Permission is granted to copy and use this program provided no fee is
   charged for it and provided that this copyright notice is not removed. */


typedef long vall[maxcategs];
typedef double contribarr[maxcategs];

AjPSeqset* seqsets = NULL;
AjPPhyloProp phyloratecat = NULL;
AjPPhyloProp phyloweights = NULL;
AjPPhyloTree* phylotrees;

ajint numseqs;
ajint numwts;

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
void   prot_inittable(void);

void   alloc_pmatrix(long);
void   getinput(void);
void   inittravtree(node *);
void   prot_nuview(node *);
void   prot_slopecurv(node *, double, double *, double *, double *);
void   makenewv(node *);
void   update(node *);
void   smooth(node *);
void   make_pmatrix(double **, double **, double **, long, double,
            double, double *, double **);
double prot_evaluate(node *, boolean);

void   treevaluate(void);
void   promlcopy(tree *, tree *, long, long);
void   proml_re_move(node **, node **);
void   insert_(node *, node *, boolean);
void   addtraverse(node *, node *, boolean);
void   rearrange(node *, node *);
void   proml_coordinates(node *, double, long *, double *);
void   proml_printree(void);
void   sigma(node *, double *, double *, double *);
void   describe(node *);

void   prot_reconstr(node *, long);
void   rectrav(node *, long, long);
void   summarize(void);
void   initpromlnode(node **, node **, node *, long, long, long *, long *,
            initops, pointarray, pointarray, Char *, Char *, char**);
void   dnaml_treeout(node *);
void   buildnewtip(long, tree *);
void   buildsimpletree(tree *);
void   free_all_protx (long, pointarray);
void   maketree(void);
void   clean_up(void);

/* function prototypes */
#endif

extern sequence y;

long rcategs;


Char infilename[100], intreename[100],
     catfilename[100], weightfilename[100];
const char* outfilename;
const char* outtreename;
double *rate, *rrate, *probcat;
long nonodes2, sites, weightsum, categs,
  datasets, ith, njumble, jumb;
long inseed, inseed0, parens;
boolean global, jumble, weights, trout, usertree, reconsider,
  ctgry, rctgry, auto_, hypstate, progress, mulsets, justwts, firstset,
  improve, smoothit, polishing, lngths, gama, invar, usepam, usejtt;
tree curtree, bestree, bestree2, priortree;
node *qwhere, *grbg;
double cv, alpha, lambda, invarfrac, bestyet;
long *enterorder;
steptr aliasweight;
contribarr *contribution, like, nulike, clai;
double **term, **slopeterm, **curveterm;
longer seed;
char *progname;
char aachar[26]="ARNDCQEGHILKMFPSTWYVBZX?*-";

/* Local variables for maketree, propagated globally for c version: */
long k, nextsp, numtrees, maxwhich, mx, mx0, mx1;
double dummy, maxlogl;
boolean succeeded, smoothed;
double **l0gf;
double *l0gl;
double **tbl;
Char ch, ch2;
long col;
vall *mp;


/* Variables introduced to allow for protein probability calculations   */
long   max_num_sibs;                /* maximum number of siblings used in a */
                                /* nuview calculation.  determines size */
                                /* final size of pmatrices              */
double *eigmat;                        /* eig matrix variable                        */
double **probmat;                /* prob matrix variable                        */
double ****dpmatrix;                /* derivative of pmatrix                */
double ****ddpmatrix;           /* derivative of xpmatrix               */
double *****pmatrices;          /* matrix of probabilities of protien   */
                                /* conversion.  The 5 subscripts refer  */
                                /* to sibs, rcategs, categs, final and  */
                                /* initial states, respectively.        */
double freqaa[20];                /* amino acid frequencies                */

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

    inseed = ajAcdGetInt("seed");

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
//  boolean didchangecat, didchangercat;
//  double probsum;
//
//  fprintf(outfile, "\nAmino acid sequence Maximum Likelihood");
//  fprintf(outfile, " method, version %s\n\n",VERSION);
//  putchar('\n');
//  ctgry = false;
//  didchangecat = false;
//  rctgry = false;
//  didchangercat = false;
//  categs = 1;
//  rcategs = 1;
//  auto_ = false;
//  gama = false;
//  global = false;
//  hypstate = false;
//  improve = false;
//  invar = false;
//  jumble = false;
//  njumble = 1;
//  lngths = false;
//  lambda = 1.0;
//  outgrno = 1;
//  outgropt = false;
//  reconsider = false;
//  trout = true;
//  usertree = false;
//  weights = false;
//  printdata = false;
//  progress = true;
//  treeprint = true;
//  usepam = false;
//  usejtt = true;
//  interleaved = true;
//  init_protmats();
//  loopcount = 0;
//  for (;;){
//    cleerhome();
//    printf("Amino acid sequence Maximum Likelihood");
//    printf(" method, version %s\n\n",VERSION);
//    printf("Settings for this run:\n");
//    printf("  U                 Search for best tree?  %s\n",
//           (usertree ? "No, use user trees in input file" : "Yes"));
//    if (usertree) {
//      printf("  L          Use lengths from user trees?  %s\n",
//               (lngths ? "Yes" : "No"));
//    }
//    printf("  P   JTT or PAM amino acid change model?  %s\n",
//           (usejtt ? "Jones-Taylor-Thornton model" : "Dayhoff PAM model"));
//    printf("  C                One category of sites?");
//    if (!ctgry || categs == 1)
//      printf("  Yes\n");
//    else
//      printf("  %ld categories of sites\n", categs);
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
//    printf("  W                       Sites weighted?  %s\n",
//           (weights ? "Yes" : "No"));
//    if ((!usertree) || reconsider) {
//      printf("  S        Speedier but rougher analysis?  %s\n",
//             (improve ? "No, not rough" : "Yes"));
//      printf("  G                Global rearrangements?  %s\n",
//             (global ? "Yes" : "No"));
//    }
//    if (!usertree) {
//      printf("  J   Randomize input order of sequences?");
//      if (jumble)
//        printf("  Yes (seed =%8ld,%3ld times)\n", inseed0, njumble);
//      else
//        printf("  No. Use input order\n");
//    }
//    else
//       printf("  V    Rearrange starting with user tree?  %s\n",
//             (reconsider ? "Yes" : "No"));
//    printf("  O                        Outgroup root?  %s%3ld\n",
//           (outgropt ? "Yes, at sequence number" :
//                       "No, use as outgroup species"),outgrno);
//    printf("  M           Analyze multiple data sets?");
//    if (mulsets)
//      printf("  Yes, %2ld %s\n", datasets,
//               (justwts ? "sets of weights" : "data sets"));
//    else
//      printf("  No\n");
//    printf("  I          Input sequences interleaved?  %s\n",
//           (interleaved ? "Yes" : "No, sequential"));
//    printf("  0   Terminal type (IBM PC, ANSI, none)?  %s\n",
//           (ibmpc ? "IBM PC" : ansi  ? "ANSI" : "(none)"));
//    printf("  1    Print out the data at start of run  %s\n",
//           (printdata ? "Yes" : "No"));
//    printf("  2  Print indications of progress of run  %s\n",
//           (progress ? "Yes" : "No"));
//    printf("  3                        Print out tree  %s\n",
//           (treeprint ? "Yes" : "No"));
//    printf("  4       Write out trees onto tree file?  %s\n",
//           (trout ? "Yes" : "No"));
//    printf("  5   Reconstruct hypothetical sequences?  %s\n",
//           (hypstate ? "Yes" : "No"));
//    printf("\n  Y to accept these or type the letter for one to change\n");
//#ifdef WIN32
//    phyFillScreenColor();
//#endif
//    scanf("%c%*[^\n]", &ch);
//    getchar();
//    if (ch == '\n')
//      ch = ' ';
//    uppercase(&ch);
//    if (ch == 'Y')
//      break;
//    if (strchr("UPLCRAWSGJVOMI012345",ch) != NULL){
//      switch (ch) {
//
//      case 'C':
//        ctgry = !ctgry;
//        if (ctgry) {
//          printf("\nSitewise user-assigned categories:\n\n");
//          initcatn(&categs);
//          if (rate){
//            free(rate);
//          }
//          rate    = (double *) Malloc(categs * sizeof(double));
//          didchangecat = true;
//          initcategs(categs, rate);
//        }
//        break;
//
//      case 'P':
//        if (usejtt) {
//          usejtt = false;
//          usepam = true;
//        } else {
//            usepam = false;
//            usejtt = true;
//        }
//        break;
//
//      case 'R':
//        if (!rctgry) {
//          rctgry = true;
//          gama = true;
//        } else {
//          if (gama) {
//            gama = false;
//            invar = true;
//          } else {
//            if (invar)
//              invar = false;
//            else
//              rctgry = false;
//          }
//        }
//        break;
//
//      case 'A':
//        auto_ = !auto_;
//        if (auto_)
//          initlambda(&lambda);
//        break;
//
//      case 'W':
//        weights = !weights;
//        break;
//
//      case 'S':
//        improve = !improve;
//        break;
//
//      case 'G':
//        global = !global;
//        break;
//
//      case 'J':
//        jumble = !jumble;
//        if (jumble)
//          initjumble(&inseed, &inseed0, seed, &njumble);
//        else njumble = 1;
//        break;
//
//      case 'L':
//        lngths = !lngths;
//        break;
//
//      case 'O':
//        outgropt = !outgropt;
//        if (outgropt)
//          initoutgroup(&outgrno, spp);
//        break;
//
//      case 'U':
//        usertree = !usertree;
//        break;
//
//      case 'V':
//        reconsider = !reconsider;
//        break;
//
//      case 'M':
//        mulsets = !mulsets;
//        if (mulsets) {
//          printf("Multiple data sets or multiple weights?");
//          loopcount2 = 0;
//          do {
//            printf(" (type D or W)\n");
//#ifdef WIN32
//            phyFillScreenColor();
//#endif
//            scanf("%c%*[^\n]", &ch2);
//            getchar();
//            if (ch2 == '\n')
//                ch2 = ' ';
//            uppercase(&ch2);
//            countup(&loopcount2, 10);
//          } while ((ch2 != 'W') && (ch2 != 'D'));
//          justwts = (ch2 == 'W');
//          if (justwts)
//            justweights(&datasets);
//          else
//            initdatasets(&datasets);
//          if (!jumble) {
//            jumble = true;
//            initjumble(&inseed, &inseed0, seed, &njumble);
//          }
//        }
//        break;
//
//      case 'I':
//        interleaved = !interleaved;
//        break;
//
//      case '0':
//        initterminal(&ibmpc, &ansi);
//        break;
//
//      case '1':
//        printdata = !printdata;
//        break;
//
//      case '2':
//        progress = !progress;
//        break;
//
//      case '3':
//        treeprint = !treeprint;
//        break;
//
//      case '4':
//        trout = !trout;
//        break;
//
//      case '5':
//        hypstate = !hypstate;
//        break;
//      }
//    } else
//      printf("Not a possible option!\n");
//    countup(&loopcount, 100);
//  }
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
//    rrate      = (double *) Malloc(rcategs*sizeof(double));
//    probcat    = (double *) Malloc(rcategs*sizeof(double));
//    rrate[0]   = 1.0;
//    probcat[0] = 1.0;
//  }
//  if (!didchangecat) {
//    rate       = (double *) Malloc(categs*sizeof(double));
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

  y = (Char **) Malloc(spp*sizeof(Char *));
  for (i = 0; i < spp; i++)
    y[i] = (Char *) Malloc(sites*sizeof(Char));
  nayme       = (naym *) Malloc(spp*sizeof(naym));
  enterorder  = (long *) Malloc(spp*sizeof(long));
  category    = (long *) Malloc(sites*sizeof(long));
  weight      = (long *) Malloc(sites*sizeof(long));
  alias       = (long *) Malloc(sites*sizeof(long));
  ally        = (long *) Malloc(sites*sizeof(long));
  location    = (long *) Malloc(sites*sizeof(long));
  aliasweight = (long *) Malloc(sites*sizeof(long));
}  /* allocrest */


void doinit()
{ /* initializes variables */
  inputnumbersseq(seqsets[0], &spp, &sites, &nonodes2, 2);
/*  getoptions();*/
  makeprotfreqs();
  if (printdata)
    fprintf(outfile, "%2ld species, %3ld  sites\n", spp, sites);
  alloctree(&curtree.nodep, nonodes2, usertree);
  allocrest();
  if (usertree && !reconsider)
    return;
  alloctree(&bestree.nodep, nonodes2, 0);
  alloctree(&priortree.nodep, nonodes2, 0);
  if (njumble <= 1)
    return;
  alloctree(&bestree2.nodep, nonodes2, 0);
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
    inputcategsstr(phyloratecat->Str[0], 0, sites, category, categs, "ProML");
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
  sitesort2   (sites, aliasweight);
  sitecombine2(sites, aliasweight);
  sitescrunch2(sites, 1, 2, aliasweight);
  for (i = 1; i <= sites; i++) {
    if (aliasweight[i - 1] > 0)
      endsite = i;
  }
  for (i = 1; i <= endsite; i++) {
    location[alias[i - 1] - 1] = i;
    ally[alias[i - 1] - 1] = alias[i - 1];
  }
  term = (double **) Malloc(endsite * sizeof(double *));
  for (i = 0; i < endsite; i++)
     term[i] = (double *) Malloc(rcategs * sizeof(double));
  slopeterm = (double **) Malloc(endsite * sizeof(double *));
  for (i = 0; i < endsite; i++)
     slopeterm[i] = (double *) Malloc(rcategs * sizeof(double));
  curveterm = (double **) Malloc(endsite * sizeof(double *));
  for (i = 0; i < endsite; i++)
     curveterm[i] = (double *) Malloc(rcategs * sizeof(double));
  mp = (vall *) Malloc(sites*sizeof(vall));
  contribution = (contribarr *) Malloc(endsite*sizeof(contribarr));
}  /* makeweights */


void prot_makevalues(long categs, pointarray treenode, long endsite,
                        long spp, sequence y, steptr alias)
{
  /* set up fractional likelihoods at tips   */
  /* a version of makevalues2 found in seq.c */
  /* used by proml                             */
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

        case 'X':                /* unknown aa                            */
          for (b = 0; b <= 19; b++)
            treenode[i]->protx[k][l][b] = 1.0;
          break;

        case '?':                /* unknown aa                            */
          for (b = 0; b <= 19; b++)
            treenode[i]->protx[k][l][b] = 1.0;
          break;

        case '*':                /* stop codon symbol                    */
          for (b = 0; b <= 19; b++)
            treenode[i]->protx[k][l][b] = 1.0;
          break;

        case '-':                /* deletion event-absent data or aa */
          for (b = 0; b <= 19; b++)
            treenode[i]->protx[k][l][b] = 1.0;
          break;
        }
      }
    }
  }
}  /* prot_makevalues */


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


void prot_inittable()
{
  /* Define a lookup table. Precompute values and print them out in tables */
  /* Allocate memory for the pmatrices, dpmatices and ddpmatrices          */
  long i, j, k, l;
  double sumrates;

  /* Allocate memory for pmatrices, the array of pointers to pmatrices     */

  pmatrices = (double *****) Malloc ( spp * sizeof(double ****));

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
  if (gama) {
    fprintf(outfile, "\nDiscrete approximation to gamma distributed rates\n");
    fprintf(outfile,
    " Coefficient of variation of rates = %f  (alpha = %f)\n",
      cv, alpha);
  }
  if (rcategs > 1) {
    fprintf(outfile, "\nStates in HMM   Rate of change    Probability\n\n");
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
    if (auto_)
      fprintf(outfile,
     "Expected length of a patch of sites having the same rate = %8.3f\n",
             1/lambda);
    putc('\n', outfile);
  }
  if (categs > 1) {
    fprintf(outfile, "\nSite category   Rate of change\n\n");
    for (k = 0; k < categs; k++)
      fprintf(outfile, "%9ld%16.3f\n", k+1, rate[k]);
  }
  if ((rcategs  > 1) || (categs >> 1))
    fprintf(outfile, "\n\n");
}  /* prot_inittable */


void getinput()
{
  /* reads the input data */
  if (!justwts || firstset)
    inputoptions();
  if (!justwts || firstset)
    input_protdata(seqsets[ith-1], sites);
  makeweights();
  setuptree2(curtree);
  if (!usertree || reconsider) {
    setuptree2(bestree);
    setuptree2(priortree);
    if (njumble > 1)
      setuptree2(bestree2);
  }
  prot_allocx(nonodes2, rcategs, curtree.nodep, usertree);
  if (!usertree || reconsider) {
    prot_allocx(nonodes2, rcategs, bestree.nodep, 0);
    prot_allocx(nonodes2, rcategs, priortree.nodep, 0);
    if (njumble > 1)
      prot_allocx(nonodes2, rcategs, bestree2.nodep, 0);
  }
  prot_makevalues(rcategs, curtree.nodep, endsite, spp, y, alias);
}  /* getinput */


void inittravtree(node *p)
{
  /* traverse tree to set initialized and v to initial values */

  p->initialized = false;
  p->back->initialized = false;
  if (!p->tip) {
    inittravtree(p->next->back);
    inittravtree(p->next->next->back);
  }
}  /* inittravtree */


void prot_nuview(node *p)
{
  long i, j, k, l, m, num_sibs, sib_index;
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

    if (!sib_back_ptr->tip &&
        !sib_back_ptr->initialized)
      prot_nuview(sib_back_ptr);
  }

  /* Make pmatrices for all possible combinations of category, rcateg      */
  /* and sib                                                                   */
  sib_ptr = p;                                /* return to p */
  for (sib_index=0; sib_index < num_sibs; sib_index++) {
    sib_ptr      = sib_ptr->next;
    sib_back_ptr = sib_ptr->back;

    lw = sib_back_ptr->v;

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

      sib_ptr = p;                        /* return to p */
      /* loop through all sibs and calculate likelihoods for all possible*/
      /* amino acid combinations                                         */
      for (sib_index=0; sib_index < num_sibs; sib_index++) {
        sib_ptr      = sib_ptr->next;
        sib_back_ptr = sib_ptr->back;

        memcpy(x2, sib_back_ptr->protx[i][j], sizeof(psitelike));
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


void prot_slopecurv(node *p,double y,double *like,double *slope,double *curve)
{
  /* compute log likelihood, slope and curvature at node p */
  long i, j, k, l, m, lai;
  double sum, sumc, sumterm, lterm, sumcs, sumcc, sum2, slope2, curve2;
  double frexm = 0;                        /* frexm = freqaa[m]*x1[m] */
                                        /* frexml = frexm*x2[l]    */
  double prod4m, prod5m, prod6m;        /* elements of prod4-5 for */
                                        /* each m                   */
  double **pmat, **dpmat, **ddpmat;        /* local pointers to global*/
                                        /* matrices                   */
  double prod4, prod5, prod6;
  contribarr thelike, nulike, nuslope, nucurve,
    theslope, thecurve, clai, cslai, cclai;
  node *q;
  psitelike x1, x2;

  q = p->back;
  sum = 0.0;
  for (j = 0; j < rcategs; j++) {
    for (k = 0; k < categs; k++) {
      make_pmatrix(pmatrices[0][j][k], dpmatrix[j][k], ddpmatrix[j][k],
                                        2, y, tbl[j][k], eigmat, probmat);
    }
  }
  for (i = 0; i < endsite; i++) {
    k = category[alias[i]-1] - 1;
    for (j = 0; j < rcategs; j++) {
      memcpy(x1, p->protx[i][j], sizeof(psitelike));
      memcpy(x2, q->protx[i][j], sizeof(psitelike));
      pmat = pmatrices[0][j][k];
      dpmat = dpmatrix[j][k];
      ddpmat = ddpmatrix[j][k];
      prod4 = 0.0;
      prod5 = 0.0;
      prod6 = 0.0;
      for (m = 0; m <= 19; m++) {
        prod4m = 0.0;
        prod5m = 0.0;
        prod6m = 0.0;
        frexm = x1[m] * freqaa[m];
        for (l = 0; l <= 19; l++) {
          prod4m += x2[l] * pmat[m][l];
          prod5m += x2[l] * dpmat[m][l];
          prod6m += x2[l] * ddpmat[m][l];
        }
        prod4 += frexm * prod4m;
        prod5 += frexm * prod5m;
        prod6 += frexm * prod6m;
      }
      term[i][j] = prod4;
      slopeterm[i][j] = prod5;
      curveterm[i][j] = prod6;
    }
    sumterm = 0.0;
    for (j = 0; j < rcategs; j++)
      sumterm += probcat[j] * term[i][j];
    if (sumterm < 0.0)
        sumterm = 0.00000001;        /* ??? */
    lterm = log(sumterm);
    for (j = 0; j < rcategs; j++) {
      term[i][j] = term[i][j] / sumterm;
      slopeterm[i][j] = slopeterm[i][j] / sumterm;
      curveterm[i][j] = curveterm[i][j] / sumterm;
    }
    sum += (aliasweight[i] * lterm);
  }
  for (i = 0; i < rcategs; i++) {
    thelike[i] = 1.0;
    theslope[i] = 0.0;
    thecurve[i] = 0.0;
  }
  for (i = 0; i < sites; i++) {
    sumc = 0.0;
    sumcs = 0.0;
    sumcc = 0.0;
    for (k = 0; k < rcategs; k++) {
      sumc += probcat[k] * thelike[k];
      sumcs += probcat[k] * theslope[k];
      sumcc += probcat[k] * thecurve[k];
    }
    sumc *= lambda;
    sumcs *= lambda;
    sumcc *= lambda;
    if ((ally[i] > 0) && (location[ally[i]-1] > 0)) {
      lai = location[ally[i] - 1];
      memcpy(clai, term[lai - 1], rcategs*sizeof(double));
      memcpy(cslai, slopeterm[lai - 1], rcategs*sizeof(double));
      memcpy(cclai, curveterm[lai - 1], rcategs*sizeof(double));
      if (weight[i] > 1) {
        for (j = 0; j < rcategs; j++) {
          if (clai[j] > 0.0)
            clai[j] = exp(weight[i]*log(clai[j]));
          else clai[j] = 0.0;
          if (cslai[j] > 0.0)
            cslai[j] = exp(weight[i]*log(cslai[j]));
          else cslai[j] = 0.0;
          if (cclai[j] > 0.0)
            cclai[j] = exp(weight[i]*log(cclai[j]));
          else cclai[j] = 0.0;
        }
      }
      for (j = 0; j < rcategs; j++) {
        nulike[j]  = ((1.0 - lambda) * thelike[j]  + sumc) *  clai[j];
        nuslope[j] = ((1.0 - lambda) * theslope[j] + sumcs) * clai[j]
                   + ((1.0 - lambda) * thelike[j]  + sumc) *  cslai[j];
        nucurve[j] = ((1.0 - lambda) * thecurve[j] + sumcc) * clai[j]
             + 2.0 * ((1.0 - lambda) * theslope[j] + sumcs) * cslai[j]
                   + ((1.0 - lambda) * thelike[j]  + sumc) *  cclai[j];
      }
    } else {
      for (j = 0; j < rcategs; j++) {
        nulike[j]  = ((1.0 - lambda) * thelike[j]  + sumc);
        nuslope[j] = ((1.0 - lambda) * theslope[j] + sumcs);
        nucurve[j] = ((1.0 - lambda) * thecurve[j] + sumcc);
      }
    }
    memcpy(thelike, nulike, rcategs*sizeof(double));
    memcpy(theslope, nuslope, rcategs*sizeof(double));
    memcpy(thecurve, nucurve, rcategs*sizeof(double));
  }
  sum2 = 0.0;
  slope2 = 0.0;
  curve2 = 0.0;
  for (i = 0; i < rcategs; i++) {
    sum2 += probcat[i] * thelike[i];
    slope2 += probcat[i] * theslope[i];
    curve2 += probcat[i] * thecurve[i];
  }
  sum += log(sum2);
  (*like) = sum;
  (*slope) = slope2 / sum2;
  (*curve) = (curve2 - slope2 * slope2 / sum2) / sum2;
} /* prot_slopecurv */


void makenewv(node *p)
{
  /* Newton-Raphson algorithm improvement of a branch length */
  long it, ite;
  double y, yold=0, yorig, like, slope, curve, oldlike=0;
  boolean done, firsttime, better;
  node *q;

  q = p->back;
  y = p->v;
  yorig = y;
  done = false;
  firsttime = true;
  it = 1;
  ite = 0;
  while ((it < iterations) && (ite < 20) && (!done)) {
    prot_slopecurv(p, y, &like, &slope, &curve);
    better = false;
    if (firsttime) {
      yold = y;
      oldlike = like;
      firsttime = false;
      better = true;
    } else {
      if (like > oldlike) {
        yold = y;
        oldlike = like;
        better = true;
        it++;
      }
    }
    if (better) {
      y = y + slope/fabs(curve);
      if (y < epsilon)
        y = epsilon;
    } else {
      if (fabs(y - yold) < epsilon)
        ite = 20;
      y = (y + (7 * yold)) / 8;
    }
    ite++;
    done = fabs(y-yold) < epsilon;
  }
  smoothed = (fabs(yold-yorig) < epsilon) && (yorig > 1000.0*epsilon);
  p->v = yold;
  q->v = yold;
  curtree.likelihood = oldlike;
}  /* makenewv */


void update(node *p)
{
  long num_sibs, i;
  node *sib_ptr;

  if (!p->tip && !p->initialized)
    prot_nuview(p);
  if (!p->back->tip && !p->back->initialized)
    prot_nuview(p->back);
  if (p->iter) {
    makenewv(p);

    if (!p->tip) {
      num_sibs = count_sibs(p);
      sib_ptr  = p;
      for (i=0; i < num_sibs; i++) {
        sib_ptr              = sib_ptr->next;
        sib_ptr->initialized = false;
      }
    }

    if (!p->back->tip) {
      num_sibs = count_sibs(p->back);
      sib_ptr  = p->back;
      for (i=0; i < num_sibs; i++) {
        sib_ptr              = sib_ptr->next;
        sib_ptr->initialized = false;
      }
    }
  }
}  /* update */


void smooth(node *p)
{
  long i, num_sibs;
  node *sib_ptr;

  smoothed = false;
  update(p);
  if (p->tip)
    return;

  num_sibs = count_sibs(p);
  sib_ptr  = p;

  for (i=0; i < num_sibs; i++) {
    sib_ptr = sib_ptr->next;

    if (polishing || (smoothit && !smoothed)) {
      smooth(sib_ptr->back);
      p->initialized = false;
      sib_ptr->initialized = false;
    }
  }
}  /* smooth */


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


double prot_evaluate(node *p, boolean saveit)
{
  contribarr tterm;
  double sum, sum2, sumc, y, prod4, prodl, frexm, sumterm, lterm;
  double **pmat;
  long i, j, k, l, m, lai;
  node *q;
  psitelike x1, x2;

  sum = 0.0;
  q = p->back;
  y = p->v;
  for (j = 0; j < rcategs; j++)
    for (k = 0; k < categs; k++)
      make_pmatrix(pmatrices[0][j][k],NULL,NULL,0,y,tbl[j][k],eigmat,probmat);
  for (i = 0; i < endsite; i++) {
    k = category[alias[i]-1] - 1;
    for (j = 0; j < rcategs; j++) {
      memcpy(x1, p->protx[i][j], sizeof(psitelike));
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
        sumterm = 0.00000001;        /* ??? */
    lterm = log(sumterm);
    for (j = 0; j < rcategs; j++)
      clai[j] = tterm[j] / sumterm;
    memcpy(contribution[i], clai, rcategs*sizeof(double));
    if (saveit && !auto_ && usertree)
      l0gf[which - 1][i] = lterm;
    sum += aliasweight[i] * lterm;
  }
  for (j = 0; j < rcategs; j++)
    like[j] = 1.0;
  for (i = 0; i < sites; i++) {
    sumc = 0.0;
    for (k = 0; k < rcategs; k++)
      sumc += probcat[k] * like[k];
    sumc *= lambda;
    if ((ally[i] > 0) && (location[ally[i]-1] > 0)) {
      lai = location[ally[i] - 1];
      memcpy(clai, contribution[lai - 1], rcategs*sizeof(double));
      for (j = 0; j < rcategs; j++)
        nulike[j] = ((1.0 - lambda) * like[j] + sumc) * clai[j];
    } else {
      for (j = 0; j < rcategs; j++)
        nulike[j] = ((1.0 - lambda) * like[j] + sumc);
    }
    memcpy(like, nulike, rcategs*sizeof(double));
  }
  sum2 = 0.0;
  for (i = 0; i < rcategs; i++)
    sum2 += probcat[i] * like[i];
  sum += log(sum2);
  curtree.likelihood = sum;
  if (!saveit || auto_ || !usertree)
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


void treevaluate()
{
  /* evaluate a user tree */
  long i;

  inittravtree(curtree.start);
  polishing = true;
  smoothit = true;
  for (i = 1; i <= smoothings * 4; i++)
    smooth (curtree.start);
  dummy = prot_evaluate(curtree.start, true);
}  /* treevaluate */


void promlcopy(tree *a, tree *b, long nonodes, long categs)
{
  /* used in proml */
  long i, j=0;
  node *p, *q;

  for (i = 0; i < spp; i++) {
    prot_copynode(a->nodep[i], b->nodep[i], categs);
    if (a->nodep[i]->back) {
      if (a->nodep[i]->back == a->nodep[a->nodep[i]->back->index - 1])
        b->nodep[i]->back = b->nodep[a->nodep[i]->back->index - 1];
      else if (a->nodep[i]->back == a->nodep[a->nodep[i]->back->index - 1]->next
)
        b->nodep[i]->back = b->nodep[a->nodep[i]->back->index - 1]->next;
      else
        b->nodep[i]->back = b->nodep[a->nodep[i]->back->index - 1]->next->next;
    }
    else b->nodep[i]->back = NULL;
  }
  for (i = spp; i < nonodes; i++) {
    p = a->nodep[i];
    q = b->nodep[i];
    for (j = 1; j <= 3; j++) {
      prot_copynode(p, q, categs);
      if (p->back) {
        if (p->back == a->nodep[p->back->index - 1])
          q->back = b->nodep[p->back->index - 1];
        else if (p->back == a->nodep[p->back->index - 1]->next)
          q->back = b->nodep[p->back->index - 1]->next;
        else
          q->back = b->nodep[p->back->index - 1]->next->next;
      }
      else
        q->back = NULL;
      p = p->next;
      q = q->next;
    }
  }
  b->likelihood = a->likelihood;
  b->start = a->start;               /* start used in dnaml only */
  b->root = a->root;                 /* root used in dnamlk only */
}  /* promlcopy */


void proml_re_move(node **p, node **q)
{
  /* remove p and record in q where it was */
  long i;

  /** assumes bifurcations */
  *q = (*p)->next->back;
  hookup(*q, (*p)->next->next->back);
  (*p)->next->back = NULL;
  (*p)->next->next->back = NULL;
  inittrav((*q));
  inittrav((*q)->back);
  i = 1;
  while (i <= smoothings) {
    smooth(*q);
    if (smoothit)
      smooth((*q)->back);
    i++;
  }
}  /* proml_re_move */


void insert_(node *p, node *q, boolean dooinit)
{
  /* Insert q near p */
  long i, j, num_sibs;
  node *r, *sib_ptr;

  r = p->next->next;
  hookup(r, q->back);
  hookup(p->next, q);
  q->v = 0.5 * q->v;
  q->back->v = q->v;
  r->v = q->v;
  r->back->v = r->v;
  p->initialized = false;
  p->next->initialized = false;
  p->next->next->initialized = false;
  if (dooinit) {
    inittrav(p);
    inittrav(q);
    inittrav(q->back);
  }
  i = 1;
  while (i <= smoothings) {
    smooth(p);
    if (!smoothit) {
      if (!p->tip) {
        num_sibs = count_sibs(p);
        sib_ptr  = p;
        for (j=0; j < num_sibs; j++) {
          smooth(sib_ptr->next->back);
          sib_ptr = sib_ptr->next;
        }
      }
    }
    else
      smooth(p->back);
    i++;
  }
}  /* insert_ */


void addtraverse(node *p, node *q, boolean contin)
{
  /* try adding p at q, proceed recursively through tree */
  long i, num_sibs;
  double like, vsave = 0;
  node *qback = NULL, *sib_ptr;

  if (!smoothit) {
    vsave = q->v;
    qback = q->back;
  }
  insert_(p, q, false);
  like = prot_evaluate(p, false);
  if (like > bestyet || bestyet == UNDEFINED) {
    bestyet = like;
    if (smoothit)
      promlcopy(&curtree, &bestree, nonodes2, rcategs);
    else
      qwhere = q;
    succeeded = true;
  }
  if (smoothit)
    promlcopy(&priortree, &curtree, nonodes2, rcategs);
  else {
    hookup (q, qback);
    q->v = vsave;
    q->back->v = vsave;
    curtree.likelihood = bestyet;
  }
  if (!q->tip && contin) {
    num_sibs = count_sibs(q);
    if (q == curtree.start)
      num_sibs++;
    sib_ptr  = q;
    for (i=0; i < num_sibs; i++) {
      addtraverse(p, sib_ptr->next->back, contin);
      sib_ptr = sib_ptr->next;
    }
  }
}  /* addtraverse */


void rearrange(node *p, node *pp)
{
  /* rearranges the tree, globally or locally moving pp around near p */
  long i, num_sibs;
  double v3 = 0, v4 = 0, v5 = 0;
  node *q, *r, *sib_ptr;

  if (!p->tip && !p->back->tip) {
    curtree.likelihood = bestyet;
    if (p->back->next != pp)
      r = p->back->next;
    else
      r = p->back->next->next;
    /* assumes bifurcations? */
    if (!smoothit) {
      v3 = r->v;
      v4 = r->next->v;
      v5 = r->next->next->v;
    }
    else
      promlcopy(&curtree, &bestree, nonodes2, rcategs);
    proml_re_move(&r, &q);
    if (smoothit)
      promlcopy(&curtree, &priortree, nonodes2, rcategs);
    else
      qwhere = q;
    num_sibs = count_sibs (p);
    sib_ptr  = p;
    for (i=0; i < num_sibs; i++) {
      sib_ptr = sib_ptr->next;
      addtraverse(r, sib_ptr->back, (boolean)(global && (nextsp == spp)));
    }
    if (global && nextsp == spp && !succeeded) {
      p = p->back;
      if (!p->tip) {
        num_sibs = count_sibs (p);
        sib_ptr  = p;
        for (i=0; i < num_sibs; i++) {
          sib_ptr = sib_ptr->next;
          addtraverse(r, sib_ptr->back, (boolean)(global && (nextsp == spp)));
        }
      }
      p = p->back;
    }
    if (smoothit)
      promlcopy(&bestree, &curtree, nonodes2, rcategs);
    else {
      insert_(r, qwhere, true);
      if (qwhere == q) {
        r->v = v3;
        r->back->v = v3;
        r->next->v = v4;
        r->next->back->v = v4;
        r->next->next->v = v5;
        r->next->next->back->v = v5;
        curtree.likelihood = bestyet;
      }
      else {
        smoothit = true;
        for (i = 1; i<=smoothings; i++) {
          smooth(r);
          smooth(r->back);
        }
        smoothit = false;
        promlcopy(&curtree, &bestree, nonodes2, rcategs);
      }
    }
    if (global && nextsp == spp && progress) {
      putchar('.');
      fflush(stdout);
    }
  }
  if (!p->tip) {
    num_sibs = count_sibs(p);
    if (p == curtree.start)
      num_sibs++;
    sib_ptr  = p;
    for (i=0; i < num_sibs; i++) {
      sib_ptr = sib_ptr->next;
      rearrange(sib_ptr->back, p);
    }
  }
}  /* rearrange */


void proml_coordinates(node *p, double lengthsum, long *tipy,
                        double *tipmax)
{
  /* establishes coordinates of nodes */
  node *q, *first, *last;
  double xx;

  if (p->tip) {
    p->xcoord = (long)(over * lengthsum + 0.5);
    p->ycoord = (*tipy);
    p->ymin = (*tipy);
    p->ymax = (*tipy);
    (*tipy) += down;
    if (lengthsum > (*tipmax))
      (*tipmax) = lengthsum;
    return;
  }
  q = p->next;
  do {
    xx = q->v;
    if (xx > 100.0)
      xx = 100.0;
    proml_coordinates(q->back, lengthsum + xx, tipy,tipmax);
    q = q->next;
  } while ((p == curtree.start || p != q) &&
           (p != curtree.start || p->next != q));
  first = p->next->back;
  q = p;
  while (q->next != p)
    q = q->next;
  last = q->back;
  p->xcoord = (long)(over * lengthsum + 0.5);
  if (p == curtree.start)
    p->ycoord = p->next->next->back->ycoord;
  else
    p->ycoord = (first->ycoord + last->ycoord) / 2;
  p->ymin = first->ymin;
  p->ymax = last->ymax;
}  /* proml_coordinates */


void proml_printree()
{
  /* prints out diagram of the tree2 */
  long tipy;
  double scale, tipmax;
  long i;

  if (!treeprint)
    return;
  putc('\n', outfile);
  tipy = 1;
  tipmax = 0.0;
  proml_coordinates(curtree.start, 0.0, &tipy, &tipmax);
  scale = 1.0 / (long)(tipmax + 1.000);
  for (i = 1; i <= (tipy - down); i++)
    drawline2(i, scale, curtree);
  putc('\n', outfile);
}  /* proml_printree */


void sigma(node *p, double *sumlr, double *s1, double *s2)
{
  /* compute standard deviation */
  double tt, aa, like, slope, curv;

  prot_slopecurv(p, p->v, &like, &slope, &curv);
  tt = p->v;
  p->v = epsilon;
  p->back->v = epsilon;
  aa = prot_evaluate(p, false);
  p->v = tt;
  p->back->v = tt;
  (*sumlr) = prot_evaluate(p, false) - aa;
  if (curv < -epsilon) {
    (*s1) = p->v + (-slope - sqrt(slope * slope -  3.841 * curv)) / curv;
    (*s2) = p->v + (-slope + sqrt(slope * slope -  3.841 * curv)) / curv;
  }
  else {
    (*s1) = -1.0;
    (*s2) = -1.0;
  }
}  /* sigma */


void describe(node *p)
{
  /* print out information for one branch */
  long i, num_sibs;
  node *q, *sib_ptr;
  double sumlr, sigma1, sigma2;

  if (!p->tip && !p->initialized)
    prot_nuview(p);
  if (!p->back->tip && !p->back->initialized)
    prot_nuview(p->back);
  q = p->back;
  if (q->tip) {
    fprintf(outfile, " ");
    for (i = 0; i < nmlngth; i++)
      putc(nayme[q->index-1][i], outfile);
    fprintf(outfile, "    ");
  } else
    fprintf(outfile, "  %4ld          ", q->index - spp);
  if (p->tip) {
    for (i = 0; i < nmlngth; i++)
      putc(nayme[p->index-1][i], outfile);
  } else
    fprintf(outfile, "%4ld      ", p->index - spp);
  fprintf(outfile, "%15.5f", q->v);
  if (p->iter) {
    sigma(q, &sumlr, &sigma1, &sigma2);
    if (sigma1 <= sigma2)
      fprintf(outfile, "     (     zero,    infinity)");
    else {
      fprintf(outfile, "     (");
      if (sigma2 <= 0.0)
        fprintf(outfile, "     zero");
      else
        fprintf(outfile, "%9.5f", sigma2);
      fprintf(outfile, ",%12.5f", sigma1);
      putc(')', outfile);
      }
    if (sumlr > 1.9205)
      fprintf(outfile, " *");
    if (sumlr > 2.995)
      putc('*', outfile);
    }
  putc('\n', outfile);
  if (!p->tip) {
    num_sibs = count_sibs(p);
    sib_ptr  = p;
    for (i=0; i < num_sibs; i++) {
      sib_ptr = sib_ptr->next;
      describe(sib_ptr->back);
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
  long i;

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
    rectrav(p->next->back, m, n);
    rectrav(p->next->next->back, m, n);
  }
  mx1 = mx;
}  /* rectrav */


void summarize()
{
  /* print out branch length information and node numbers */
  long i, j, mm=0, num_sibs;
  double mode, sum;
  double like[maxcategs],nulike[maxcategs];
  double **marginal;
  node   *sib_ptr;

  if (!treeprint)
    return;
  fprintf(outfile, "\nremember: ");
  if (outgropt)
    fprintf(outfile, "(although rooted by outgroup) ");
  fprintf(outfile, "this is an unrooted tree!\n\n");
  fprintf(outfile, "Ln Likelihood = %11.5f\n", curtree.likelihood);
  fprintf(outfile, "\n Between        And            Length");
  if (!(usertree && lngths))
    fprintf(outfile, "      Approx. Confidence Limits");
  fprintf(outfile, "\n");
  fprintf(outfile, " -------        ---            ------");
  if (!(usertree && lngths))
    fprintf(outfile, "      ------- ---------- ------");
  fprintf(outfile, "\n\n");
  for (i = spp; i < nonodes2; i++) {
    /* So this works with arbitrary multifurcations */
    if (curtree.nodep[i]) {
      num_sibs = count_sibs (curtree.nodep[i]);
      sib_ptr  = curtree.nodep[i];
      for (j = 0; j < num_sibs; j++) {
        sib_ptr->initialized = false;
        sib_ptr              = sib_ptr->next;
      }
    }
  }

  describe(curtree.start->back);

  /* So this works with arbitrary multifurcations */
  num_sibs = count_sibs(curtree.start);
  sib_ptr  = curtree.start;
  for (i=0; i < num_sibs; i++) {
    sib_ptr = sib_ptr->next;
    describe(sib_ptr->back);
  }

  fprintf(outfile, "\n");
  if (!(usertree && lngths)) {
    fprintf(outfile, "     *  = significantly positive, P < 0.05\n");
    fprintf(outfile, "     ** = significantly positive, P < 0.01\n\n");
  }
  dummy = prot_evaluate(curtree.start, false);
  if (rctgry && rcategs > 1) {
    for (i = 0; i < rcategs; i++)
      like[i] = 1.0;
    for (i = sites - 1; i >= 0; i--) {
      sum = 0.0;
      for (j = 0; j < rcategs; j++) {
        nulike[j] = (1.0 - lambda + lambda * probcat[j]) * like[j];
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
    marginal = (double **) Malloc(sites*sizeof(double *));
    for (i = 0; i < sites; i++)
      marginal[i] = (double *) Malloc(rcategs*sizeof(double));
    for (i = 0; i < rcategs; i++)
      like[i] = 1.0;
    for (i = sites - 1; i >= 0; i--) {
      sum = 0.0;
      for (j = 0; j < rcategs; j++) {
        nulike[j] = (1.0 - lambda + lambda * probcat[j]) * like[j];
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
        nulike[j] = (1.0 - lambda + lambda * probcat[j]) * like[j];
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
      rectrav(curtree.start, i, k);
      rectrav(curtree.start->back, i, k);
      putc('\n', outfile);
      mx0 = mx1;
    }
  }
}  /* summarize */


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
  default:      /* cases hslength, hsnolength, treewt, unittrwt */
    break;      /* should never occur                           */
  }
} /* initpromlnode */


void dnaml_treeout(node *p)
{
  /* write out file with representation of final tree2 */
  /* Only works for bifurcations! */
  long i, n, w;
  Char c;
  double x;

  if (p->tip) {
    n = 0;
    for (i = 1; i <= nmlngth; i++) {
      if (nayme[p->index-1][i - 1] != ' ')
        n = i;
    }
    for (i = 0; i < n; i++) {
      c = nayme[p->index-1][i];
      if (c == ' ')
        c = '_';
      putc(c, outtree);
    }
    col += n;
  } else {
    putc('(', outtree);
    col++;
    dnaml_treeout(p->next->back);
    putc(',', outtree);
    col++;
    if (col > 45) {
      putc('\n', outtree);
      col = 0;
    }
    dnaml_treeout(p->next->next->back);
    if (p == curtree.start) {
      putc(',', outtree);
      col++;
      if (col > 45) {
        putc('\n', outtree);
        col = 0;
      }
      dnaml_treeout(p->back);
    }
    putc(')', outtree);
    col++;
  }
  x = p->v;
  if (x > 0.0)
    w = (long)(0.43429448222 * log(x));
  else if (x == 0.0)
    w = 0;
  else
    w = (long)(0.43429448222 * log(-x)) + 1;
  if (w < 0)
    w = 0;
  if (p == curtree.start)
    fprintf(outtree, ";\n");
  else {
    fprintf(outtree, ":%*.5f", (int)(w + 7), x);
    col += w + 8;
  }
}  /* dnaml_treeout */


void buildnewtip(long m, tree *tr)
{
  node *p;

  p = tr->nodep[nextsp + spp - 3];
  hookup(tr->nodep[m - 1], p);
  p->v = initialv;
  p->back->v = initialv;
}  /* buildnewtip */


void buildsimpletree(tree *tr)
{
  hookup(tr->nodep[enterorder[0] - 1], tr->nodep[enterorder[1] - 1]);
  tr->nodep[enterorder[0] - 1]->v = 1.0;
  tr->nodep[enterorder[0] - 1]->back->v = 1.0;
  tr->nodep[enterorder[1] - 1]->v = 1.0;
  tr->nodep[enterorder[1] - 1]->back->v = 1.0;
  buildnewtip(enterorder[2], tr);
  insert_(tr->nodep[enterorder[2] - 1]->back,
          tr->nodep[enterorder[0] - 1], false);
}  /* buildsimpletree */


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
  long i, j, k, l;
  long num_sibs;
  boolean dummy_haslengths, dummy_first, goteof;
  pointarray dummy_treenode=NULL;
  long nextnode;
  node *root, *q, *r;
  char* treestr;

  prot_inittable();

  if (usertree) {
    /*openfile(&intree,INTREE,"input tree file", "r",progname,intreename);*/
    if (numtrees > 2)
      emboss_initseed(inseed, &inseed0, seed);
    l0gl = (double *) Malloc(numtrees * sizeof(double));
    l0gf = (double **) Malloc(numtrees * sizeof(double *));
    for (i=0; i < numtrees; ++i)
      l0gf[i] = (double *) Malloc(endsite * sizeof(double));
    if (treeprint) {
      fprintf(outfile, "User-defined tree");
      if (numtrees > 1)
        putc('s', outfile);
      fprintf(outfile, ":\n\n");
    }
    which = 1;

    /* This taken out of tree read, used to be [spp-1], but referring
       to [0] produces output identical to what the pre-modified dnaml
       produced. */

    while (which <= numtrees) {

      /* These initializations required each time through the loop
         since multiple trees require re-initialization */
      dummy_haslengths = true;
      nextnode         = 0;
      dummy_first      = true;
      goteof           = false;

      treestr = ajStrStr(phylotrees[which-1]->Tree);
      treeread(&treestr, &root, dummy_treenode, &goteof, &dummy_first,
               curtree.nodep, &nextnode,
               &dummy_haslengths, &grbg, initpromlnode);
      q = root;
      r = root;
      while (!(q->next == root))
        q = q->next;
      q->next = root->next;
      root = q;
      chuck(&grbg, r);
      curtree.nodep[spp] = q;
      if (goteof && (which <= numtrees)) {
        /* if we hit the end of the file prematurely */
        printf ("\n");
        printf ("ERROR: trees missing at end of file.\n");
        printf ("\tExpected number of trees:\t\t%ld\n", numtrees);
        printf ("\tNumber of trees actually in file:\t%ld.\n\n", which - 1);
        exxit (-1);
      }

      curtree.start = curtree.nodep[0]->back;

      treevaluate();
      if (reconsider) {
        bestyet = UNDEFINED;
        succeeded = true;
        while (succeeded) {
          succeeded = false;
          rearrange(curtree.start, curtree.start->back);
        }
        treevaluate();
      }
      proml_printree();
      summarize();
      if (trout) {
        col = 0;
        dnaml_treeout(curtree.start);
      }
      which++;
    }
    FClose(intree);
    putc('\n', outfile);
    if (!auto_ && numtrees > 1 && weightsum > 1 )
      standev2(numtrees, maxwhich, 0, endsite-1, maxlogl,
               l0gl, l0gf, aliasweight, seed);
  } else {
    /* If there's no input user tree, */
    for (i = 1; i <= spp; i++)
      enterorder[i - 1] = i;
    if (jumble)
      randumize(seed, enterorder);
    if (progress) {
      printf("\nAdding species:\n");
      writename(0, 3, enterorder);
#ifdef WIN32
      phyFillScreenColor();
#endif
    }
    nextsp = 3;
    polishing = false;
    buildsimpletree(&curtree);
    curtree.start = curtree.nodep[enterorder[0] - 1]->back;
    smoothit = improve;
    nextsp = 4;
    while (nextsp <= spp) {
      buildnewtip(enterorder[nextsp - 1], &curtree);
      bestyet = UNDEFINED;
      if (smoothit)
        promlcopy(&curtree, &priortree, nonodes2, rcategs);
      addtraverse(curtree.nodep[enterorder[nextsp - 1] - 1]->back,
                  curtree.start, true);
      if (smoothit)
        promlcopy(&bestree, &curtree, nonodes2, rcategs);
      else {
        insert_(curtree.nodep[enterorder[nextsp - 1] - 1]->back, qwhere, true);
        smoothit = true;
        for (i = 1; i<=smoothings; i++) {
          smooth(curtree.start);
          smooth(curtree.start->back);
        }
        smoothit = false;
        promlcopy(&curtree, &bestree, nonodes2, rcategs);
        bestyet = curtree.likelihood;
      }
      if (progress) {
        writename(nextsp - 1, 1, enterorder);
#ifdef WIN32
        phyFillScreenColor();
#endif
      }
      if (global && nextsp == spp && progress) {
        printf("Doing global rearrangements\n");
        printf("  !");
        for (j = 1; j <= (spp - 3); j++)
          putchar('-');
        printf("!\n");
#ifdef WIN32
        phyFillScreenColor();
#endif
      }
      succeeded = true;
      while (succeeded) {
        succeeded = false;
        if (global && nextsp == spp && progress) {
          printf("   ");
          fflush(stdout);
        }
        rearrange(curtree.start, curtree.start->back);
        if (global && nextsp == spp && progress)
          putchar('\n');
      }
      for (i = spp; i < nextsp + spp - 2; i++) {
        curtree.nodep[i]->initialized = false;
        curtree.nodep[i]->next->initialized = false;
        curtree.nodep[i]->next->next->initialized = false;
      }
      if (!smoothit) {
        smoothit = true;
        for (i = 1; i<=smoothings; i++) {
          smooth(curtree.start);
          smooth(curtree.start->back);
        }
        smoothit = false;
        promlcopy(&curtree, &bestree, nonodes2, rcategs);
        bestyet = curtree.likelihood;
      }
      nextsp++;
    }
    if (global && progress) {
      putchar('\n');
      fflush(stdout);
#ifdef WIN32
      phyFillScreenColor();
#endif
    }
    if (njumble > 1) {
      if (jumb == 1)
        promlcopy(&bestree, &bestree2, nonodes2, rcategs);
      else
        if (bestree2.likelihood < bestree.likelihood)
          promlcopy(&bestree, &bestree2, nonodes2, rcategs);
    }
    if (jumb == njumble) {
      if (njumble > 1)
        promlcopy(&bestree2, &curtree, nonodes2, rcategs);
      curtree.start = curtree.nodep[outgrno - 1]->back;
      for (i = 0; i < nonodes2; i++) {
        if (i < spp)
          curtree.nodep[i]->initialized = false;
        else {
          curtree.nodep[i]->initialized = false;
          curtree.nodep[i]->next->initialized = false;
          curtree.nodep[i]->next->next->initialized = false;
        }
      }
      treevaluate();
      proml_printree();
      summarize();
      if (trout) {
        col = 0;
        dnaml_treeout(curtree.start);
      }
    }
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
  for (i=0; i < endsite; i++)
     free(term[i]);
  free(term);
  for (i=0; i < endsite; i++)
     free(slopeterm[i]);
  free(slopeterm);
  for (i=0; i < endsite; i++)
     free(curveterm[i]);
  free(curveterm);
  free_all_protx(nonodes2, curtree.nodep);
  if (!usertree || reconsider) {
    free_all_protx(nonodes2, bestree.nodep);
    free_all_protx(nonodes2, priortree.nodep);
    if (njumble > 1)
      free_all_protx(nonodes2, bestree2.nodep);
  }
  if (progress) {
    printf("\n\nOutput written to file \"%s\"\n\n", outfilename);
    if (trout)
      printf("Tree also written onto file \"%s\"\n", outtreename);
    putchar('\n');
  }
}  /* maketree */


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

#if 0          /* ???? debug ???? */
  freetree2(curtree.nodep, nonodes2);

  if (! (usertree && !reconsider)) {
    freetree2(bestree.nodep, nonodes2);
    freetree2(priortree.nodep, nonodes2);
  }

  if (! (njumble <= 1))
    freetree2(bestree2.nodep, nonodes2);
#endif
  FClose(infile);
  FClose(outfile);
  FClose(outtree);
#ifdef MAC
  fixmacfile(outfilename);
  fixmacfile(outtreename);
#endif
}   /* clean_up */


int main(int argc, Char *argv[])
{  /* Protein Maximum Likelihood */
#ifdef MAC
  argc = 1;             /* macsetup("ProML","");        */
  argv[0] = "ProML";
#endif
  init(argc,argv);
  emboss_getoptions("fproml",argc,argv);
  progname = argv[0];
  /*openfile(&infile,INFILE,"input file","r",argv[0],infilename);*/
  embossoutfile = ajAcdGetOutfile("outfile");
  emboss_openfile(embossoutfile,&outfile,&outfilename);
  mulsets = false;
  datasets = 1;
  firstset = true;
  ibmpc = IBMCRT;
  ansi = ANSICRT;
  grbg = NULL;
  doinit();
/*
  if (ctgry)
    openfile(&catfile,CATFILE,"categories file","r",argv[0],catfilename);
  if (weights || justwts)
    openfile(&weightfile,WEIGHTFILE,"weights file","r",argv[0],weightfilename);
*/
  embossouttree = ajAcdGetOutfile("outtreefile");
  emboss_openfile(embossouttree,&outtree,&outtreename);
  if (!outtree) trout = false;
  for (ith = 1; ith <= datasets; ith++) {
    if (datasets > 1) {
      fprintf(outfile, "Data set # %ld:\n", ith);
      printf("\nData set # %ld:\n", ith);
    }
    getinput();
    if (ith == 1)
      firstset = false;
    for (jumb = 1; jumb <= njumble; jumb++) {
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
}  /* Protein Maximum Likelihood */

