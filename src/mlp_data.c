#include "mlp.h"

/* RMS error was 0.213312, seed was 1479227418 */
/* 0.008869 0.061371 (0.213312 0.213312) 2.44803e-06 9999 */

static const float weights[450] = {

/* hidden layer */
-1.49575f, 0.0412272f, 0.0454269f, 0.0104776f, 0.0336012f, 
-0.0455974f, -0.105047f, -0.0520699f, -0.0197964f, -0.0833037f, 
-0.0809937f, -0.0486736f, 0.0104389f, -0.0577143f, -0.00551904f, 
-0.0118415f, 0.053815f, 0.0324952f, 0.0247256f, -0.0988066f, 
-0.13444f, -0.870899f, -1.03953f, 1.18702f, -3.46052f, 
-1.54205f, 2.02356f, -0.0791162f, 0.109906f, 0.00136072f, 
0.0664953f, -0.0728222f, 0.0636893f, -0.121761f, 0.0471708f, 
-0.125624f, 0.0570126f, -0.0192511f, -0.062755f, 0.0859039f, 
0.0933374f, 0.175435f, 0.127347f, -0.18434f, -0.154779f, 
-0.152319f, 0.014854f, -1.4498f, -1.46794f, 0.0693873f, 
0.621329f, -0.0616062f, -1.66154f, 0.229979f, 0.136083f, 
0.199459f, 0.0632802f, 0.088892f, 0.0571331f, 0.0594771f, 
0.022619f, 0.231396f, 0.096896f, 0.103059f, 0.00866126f, 
-0.0567396f, -0.180445f, -0.455467f, -0.190238f, 0.137037f, 
-0.208192f, 0.0820235f, -0.162778f, -11.2296f, -2.33154f, 
0.909668f, -6.85734f, -7.73832f, -2.49902f, -0.0118603f, 
0.24852f, -0.140533f, 0.101702f, 0.0851634f, -0.149113f, 
0.076302f, 0.0621906f, -0.0984243f, 0.324914f, -0.334717f, 
-0.00670513f, -0.853213f, 0.485834f, 0.134045f, -0.418151f, 
0.350182f, 0.0269357f, -1.43603f, 0.60244f, 3.28524f, 
-4.22241f, 1.17258f, 0.217745f, 3.57593f, 0.15508f, 
-0.0671132f, 0.0475609f, -0.0598975f, 0.0191797f, -0.00831937f, 
-0.0124935f, 0.0183379f, 0.0367234f, -0.0437611f, -0.00777795f, 
-0.0300612f, -0.14059f, -0.260409f, -0.312317f, -0.541072f, 
0.00213368f, 0.152133f, -0.0629131f, 0.201782f, 0.0306692f, 
-3.09985f, 1.9477f, -1.18056f, 3.3574f, -2.27933f, 
-0.814073f, 0.0211447f, 0.0176399f, 0.0474673f, -0.0185789f, 
-0.0138199f, 0.0520541f, -0.110024f, -0.0659609f, 0.0404468f, 
-0.034577f, 0.0923533f, 0.0822167f, 0.279043f, 0.498454f, 
0.0728072f, -0.0115114f, -0.163178f, -1.44235f, -0.97958f, 
-0.193367f, 0.375702f, -0.534339f, 1.07799f, -0.0225153f, 
2.22532f, 1.74552f, -0.0041177f, -0.138695f, -0.252883f, 
-0.365422f, 0.10312f, 0.0356219f, 0.246108f, 0.0270864f, 
0.0588877f, -0.071282f, -0.328597f, 0.61501f, -0.00520579f, 
-2.17656f, -0.884142f, 0.116733f, -1.05062f, -1.05124f, 
-0.985618f, -0.52499f, -0.878724f, -2.51553f, 6.04765f, 
-5.85112f, -7.66557f, -4.34943f, -0.0170009f, 0.00138944f, 
-0.183771f, -0.0537957f, 0.000470745f, -0.0971737f, -0.0107594f, 
-0.0945811f, 0.169001f, 0.169987f, 0.105882f, 0.0825144f, 
0.113714f, -0.204991f, 0.597609f, -0.190805f, 0.283682f, 
-0.344627f, -0.521042f, 0.311584f, 11.0971f, -1.15561f, 
0.399795f, 12.8899f, -107.127f, 0.279145f, 0.00471917f, 
0.0758364f, -0.0349449f, 0.053161f, 0.0190842f, 0.0199931f, 
0.044927f, 0.0570502f, -0.111926f, -0.0118992f, 0.0106829f, 
0.0589569f, -0.0257566f, 0.334809f, -0.237388f, -0.152132f, 
-0.0354438f, -0.172024f, 0.114506f, 0.36766f, 3.71389f, 
0.887576f, -0.569327f, 4.54713f, -1.29261f, 1.06004f, 
-0.0886646f, -0.0140985f, -0.114892f, -0.032154f, -0.021562f, 
0.0145194f, -0.0269238f, 0.0241339f, -0.00411433f, -0.0111128f, 
-0.0422138f, -0.0361934f, 0.177077f, 0.160174f, 0.471734f, 
0.148651f, 0.00867273f, -0.332041f, 0.0441584f, 0.119647f, 
1.40869f, -1.26756f, 0.988608f, -1.13709f, -0.062768f, 
0.255157f, -0.0151873f, 0.0319646f, 0.122124f, 0.0517178f, 
-0.0942639f, -0.0330079f, -0.0552662f, -0.0585165f, -0.0672674f, 
0.0526535f, 0.0362472f, -0.0610607f, 0.0387742f, -0.250637f, 
0.292556f, -0.124854f, -0.115811f, 0.197886f, -0.351962f, 
-0.466052f, -1.24325f, 2.09844f, -0.294508f, -2.72454f, 
-0.504277f, 1.87673f, -0.228112f, -0.278497f, -0.482349f, 
-0.199447f, -0.0783573f, -0.0438571f, -0.221381f, 0.0300528f, 
0.0439627f, -0.00579913f, -0.194492f, 0.393551f, -0.136585f, 
0.0499079f, -0.0904164f, -0.424276f, 0.176043f, -0.335463f, 
-0.246553f, 0.110429f, 2.19957f, -1.04942f, -0.367194f, 
1.94299f, -0.47052f, 0.612852f, 0.0406875f, -0.00121944f, 
0.145926f, 0.0678098f, 0.0244709f, -0.0449821f, -0.0228517f, 
-0.0103669f, -0.0611677f, 0.0631724f, 0.0042841f, 0.0329614f, 
0.231405f, 0.168526f, 0.0176985f, -0.0178047f, -0.0571473f, 
0.0251568f, 0.392102f, 0.143365f, -0.730739f, 3.1861f, 
-0.47963f, -0.559167f, -4.98976f, -1.44878f, 0.0484166f, 
0.0468742f, -0.0270759f, -0.00987675f, 0.0865013f, 0.173479f, 
0.126091f, 0.112359f, 0.126218f, 0.15401f, 0.133092f, 
0.0592874f, -0.0610175f, -0.11337f, -0.113627f, -0.0279996f, 
0.118692f, 0.155023f, 0.11179f, 0.011755f, 0.617417f, 
-0.0348346f, 1.05458f, -0.689601f, -0.363106f, -2.09101f, 
0.0972167f, -0.30372f, 0.239327f, -0.160846f, -0.123778f, 
0.141499f, -0.196245f, 0.0764782f, 0.00367381f, -0.176602f, 
0.110237f, -0.0275133f, -0.181722f, 0.0733394f, -0.140377f, 
0.0856482f, 0.109992f, -0.192073f, -0.0623485f, 0.0986179f, 
-0.568319f, 1.42259f, 0.587944f, -0.211062f, -10.5601f, 
0.362246f, 0.00461539f, 0.100599f, 0.0724554f, 0.0488259f, 
-0.0193259f, 0.0159694f, -0.0106922f, -0.0218068f, 0.00737706f, 
0.088502f, 0.0207549f, 0.047735f, -0.124846f, 0.214096f, 
0.0925246f, 0.049763f, -0.139898f, 0.00633034f, -0.128318f, 
0.107787f, 0.824856f, 1.71267f, -0.482429f, -6.5385f, 
-2.56888f, 
/* output layer */
1.93298, -2.90743, -1.17607, -1.22319, 1.54878, 
0.871944, 2.56482, 0.374822, 0.639316, 1.68487, 
-1.21436, 1.15552, 0.304933, -1.21178, -0.79036, 
1.35881, 2.10126, 5.11079, 1.39309, -1.94099, 
-1.63519, 0.168846, 0.528794, 0.14696, -0.208921, 
-0.334534, 0.210197, 2.32808, 1.08323, -1.3599, 
0.846293, 2.14501, 0.505528, 0.918715, };

static const int topo[3] = {25, 16, 2};

const MLP net = {
	3,
	topo,
	weights
};
