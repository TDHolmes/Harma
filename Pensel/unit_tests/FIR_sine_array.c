float sine[] = {0.0, 0.00999983333417, 0.0199986666933, 0.0299955002025,
    0.0499791692707, 0.0599640064794, 0.0699428473375, 0.0799146939692,
    0.0998334166468, 0.109778300837, 0.119712207289, 0.12963414262,
    0.149438132474, 0.159318206614, 0.169182349067, 0.179029573426,
    0.198669330795, 0.208459899846, 0.218229623081, 0.227977523535,
    0.247403959255, 0.257080551892, 0.266731436689, 0.276355648564,
    0.295520206661, 0.305058636443, 0.314566560616, 0.324043028395,
    0.342897807455, 0.352274233275, 0.361615431965, 0.370920469413,
    0.389418342309, 0.398609327984, 0.40776045306, 0.416870802429,
    0.434965534111, 0.443948106966, 0.452886285379, 0.461779175541,
    0.479425538604, 0.488177246883, 0.496880137844, 0.505533341205,
    0.522687228931, 0.531186197921, 0.539632048734, 0.548023936792,
    0.564642473395, 0.5728674601, 0.581035160537, 0.589144757942,
    0.605186405736, 0.613116851973, 0.620985987037, 0.628793024018,
    0.644217687238, 0.651833771022, 0.659384671971, 0.666869635004,
    0.681638760023, 0.688921445111, 0.696135238627, 0.7032794192,
    0.7173560909, 0.72428717437, 0.731145829727, 0.73793137111,
    0.75128040514, 0.757842562895, 0.764328937026, 0.770738878899,
    0.783326909627, 0.78950373969, 0.795601620036, 0.801619940884,
    0.813415504789, 0.819191568301, 0.824885713338, 0.830497370492,
    0.841470984808, 0.846831844618, 0.852108021949, 0.857298989189,
    0.867423225594, 0.872355482345, 0.877200504275, 0.881957806885,
    0.891207360061, 0.89569868568, 0.900100442177, 0.904412189379,
    0.912763940261, 0.916803108772, 0.920750597736, 0.924606012408,
    0.932039085967, 0.935616001553, 0.939099356319, 0.942488801932,
    0.948984619356, 0.952090341591, 0.955100855585, 0.958015860289,
    0.963558185417, 0.966184951613, 0.968715100118, 0.971148377921,
    0.975723357827, 0.977864602435, 0.979908061399, 0.981853530372,
    0.985449729988, 0.987100101014, 0.988651762852, 0.990104560337,
    0.992712991038, 0.993868363412, 0.994924349778, 0.995880844538,
    0.997494986604, 0.998152472498, 0.998710143976, 0.999167945271,
    0.999783764189, 0.99994172023, 0.999999682932, 0.999957646499,
    0.999573603042, 0.999231634421, 0.998789743471, 0.998247974378,
    0.996865028454, 0.996023989917, 0.99508334981, 0.994043202198,
    0.991664810452, 0.990326804156, 0.988889766005, 0.987353839701,
    0.983985946874, 0.982154317138, 0.980224472788, 0.978196606808,
    0.973847630878, 0.971526955822, 0.96910912888, 0.966594391833,
    0.961275202975, 0.958471283079, 0.955571516853, 0.952576194272,
    0.946300087687, 0.94301993129, 0.939645473685, 0.936177052316,
    0.928959715004, 0.925211520788, 0.921370806191, 0.917437955282,
    0.909297426826, 0.905090563325, 0.900793191523, 0.896405741152,
    0.887362368633, 0.882707350816, 0.877964062999, 0.873132979508,
    0.863209366649, 0.858117829635, 0.852940481553, 0.847677840134,
    0.836898790798, 0.831383460779, 0.825784993106, 0.820103947621,
    0.80849640382, 0.802571066247, 0.796565472236, 0.790480222342,
    0.778073196888, 0.77175266202, 0.765354952529, 0.758880708181,
    0.745705212177, 0.739005278059, 0.73223144403, 0.725384387467,
    0.711473352791, 0.70441076577, 0.69727773826, 0.690074983557,
    0.675463180551, 0.668055593416, 0.660581201279, 0.653040751572,
    0.637764702135, 0.630030629996, 0.622233555319, 0.614374257806,
    0.598472144104, 0.590430918114, 0.582330649524, 0.574172148355,
    0.557683717391, 0.549355436427, 0.540972220377, 0.532534907556,
    0.515501371821, 0.506906852248, 0.498261642412, 0.489566606827,
    0.47203054129, 0.46319126493, 0.45430566983, 0.445374644542,
    0.427379880234, 0.418317940676, 0.409214169672, 0.400069477592,
    0.381660992052, 0.372399039425, 0.363099847204, 0.353764345301,
    0.334988150156, 0.325549334518, 0.316077964217, 0.306574986384,
    0.287478012343, 0.277885925817, 0.26826605093, 0.258619349661,
    0.239249329214, 0.229527947021, 0.219783612225, 0.210017299251,
    0.190422647361, 0.180596267894, 0.170751828951, 0.160890314967,
    0.14112000806, 0.13121319215, 0.121293255031, 0.111361188687,
    0.0914646422324, 0.0815021517603, 0.0715315111408, 0.0615537174299,
    0.0415806624333, 0.0315873984365, 0.0215909757261, 0.0115923939362,
    -0.00840724736715, -0.0184063069331, -0.0284035258836, -0.0383979045052,
    -0.0583741434276, -0.068354006121, -0.0783270334709, -0.0882922281826,
    -0.10819513453, -0.118130855892, -0.128054764266, -0.137965867271,
    -0.157745694143, -0.167612440044, -0.177462424841, -0.187294663543,
    -0.206901971673, -0.216675080387, -0.226426521774, -0.236155320697,
    -0.255541102027, -0.265196145872, -0.274824670323, -0.284425712536,
    -0.303541512708, -0.313054359103, -0.322535900322, -0.331985188221,
    -0.35078322769, -0.360130099472, -0.369440958544, -0.378714873829,
    -0.397148167286, -0.406305702144, -0.415422606771, -0.424497969484,
    -0.442520443295, -0.451465752161, -0.460365914829, -0.469220041289,
    -0.486786648656, -0.495497372917, -0.504158547854, -0.512769307356,
    -0.529836140908, -0.53829050829, -0.546691047069, -0.555036917199,
    -0.571561318742, -0.579738197729, -0.587857103378, -0.595917223808,
    -0.611857890943, -0.619736843595, -0.627553823079, -0.635308047704,
    -0.650625137065, -0.658186470105, -0.665681985046, -0.673110932344,
    -0.687766159184, -0.694990973216, -0.702146288731, -0.709231390201,
    -0.723188124087, -0.730058360839, -0.736855592364, -0.743579138944,
    -0.756802495308, -0.763300982767, -0.769723140764, -0.776068327088,
    -0.788525254426, -0.794635749757, -0.800666782176, -0.806617748583,
    -0.818277111064, -0.823984341212, -0.829609173611, -0.835151045785,
    -0.845983701075, -0.851273400936, -0.856477974165, -0.861596900311,
    -0.871575772414, -0.876434720492, -0.881206025828, -0.885889211297,
    -0.894989358229, -0.899405409685, -0.90373152135, -0.907967260616,
    -0.916165936749, -0.920128053756, -0.923998158723, -0.927775864645,
    -0.935052577558, -0.938550856885, -0.941955281908, -0.945265512188,
    -0.95160207389, -0.95462777166, -0.957558007449, -0.960392488236,
    -0.965773060621, -0.968318614167, -0.970767336658, -0.973118983225,
    -0.977530117665, -0.979589164428, -0.981550253092, -0.983413187547,
    -0.986843858503, -0.988411251939, -0.989879805074, -0.991249371052,
    -0.993691003633, -0.994762826075, -0.995735173062, -0.996607947362,
    -0.998054438759, -0.998628011207, -0.999101721687, -0.999475522827,
    -0.999923257564, -0.999997146388, -0.99997103633, -0.999844930002,
    -0.999292788975, -0.99886680949, -0.998340944157, -0.997715245561,
    -0.996164608836, -0.995239825769, -0.994215519549, -0.993091792606,
    -0.990546535967, -0.989125260794, -0.98760507392, -0.985986127362,
    -0.982452612624, -0.980538397794, -0.978526129941, -0.976416010291,
    -0.971903069402, -0.969500699454, -0.967001380244, -0.964405361702,
    -0.958924274663, -0.956039754271, -0.9530596307, -0.949984201961,
    -0.943548668636, -0.940189207599, -0.936735728424, -0.933188576457,
    -0.925814682328, -0.921988677548, -0.918070474669, -0.914060465508,
    -0.905766641469, -0.901483655966, -0.89711052285, -0.892647679428,
    -0.88345465572, -0.878725394728, -0.873908261929, -0.869003739032,
    -0.858934493427, -0.853770777635, -0.848521685476, -0.843187741856,
    -0.832267442224, -0.826682178232, -0.821014246711, -0.81526421445,
    -0.803520155852, -0.797527303912, -0.791454699905, -0.785302951089,
    -0.772764487556, -0.766379026676, -0.759916928532, -0.753378839328,
    -0.740077310489, -0.733315200996, -0.726479760593, -0.719571672821,
    -0.70554032557, -0.698418469216, -0.691226771597, -0.683965951877,
    -0.669239857276, -0.661776054993, -0.654246075656, -0.646650672256,
    -0.631266637872, -0.623479545279, -0.61563010525, -0.607719102724,
    -0.591715580631, -0.583624661403, -0.575475380195, -0.567268551929,
    -0.550685542598, -0.54231101982, -0.533882266392, -0.525400125182,
    -0.508279077499, -0.499641883117, -0.490954724963, -0.482218471745,
    -0.464602179414, -0.455723901915, -0.446800052405, -0.437831523263,
    -0.41976401784, -0.410666848294, -0.401528612406, -0.392350223991,
    -0.37387666483, -0.364583341424, -0.355253559988, -0.345888253492,
    -0.32705481487, -0.317588566072, -0.308090558682, -0.298561742494,
    -0.279415498199, -0.269799984702, -0.26015749143, -0.250488982627,
    -0.231077788299, -0.221337043878, -0.211574165937, -0.201790130756,
    -0.182162504272, -0.172320875716, -0.162462015215, -0.152586908649,
    -0.132791908853, -0.122873995107, -0.112943794063, -0.103002298735,
    -0.0830894028175, -0.0731199935013, -0.0631432722466, -0.0531602367174,
    -0.0331792165476, -0.0231832299924, -0.0131849251335, -0.00318530179314,
    0.0168139004843, 0.0268114795179, 0.0368063774258, 0.0467975947267,
    0.0667649915216, 0.0767391742925, 0.08670568321, 0.0966635216314,
    0.11654920485, 0.126475061096, 0.136388269942, 0.146287840073,
    0.166042105865, 0.175894826114, 0.185729957028, 0.195546515101,
    0.215119988088, 0.224874945672, 0.234607415948, 0.244316425679,
    0.263660182373, 0.273292994977, 0.282898478509, 0.29247567243,
    0.311541363513, 0.321027954123, 0.330482442205, 0.339903882319,
    0.358643853493, 0.367960510572, 0.377240371908, 0.38648250952,
    0.404849920617, 0.413973357375, 0.423055397143, 0.432095131724,
    0.450044073781, 0.458951486378, 0.467813004209, 0.476627741129,
    0.494113351139, 0.502782475682, 0.511401322396, 0.519969029404,
    0.536947602448, 0.54535677064, 0.55371140361, 0.562010665901,
    0.578439764388, 0.586567957689, 0.594637494682, 0.602647568422,
    0.618486128163, 0.626313030322, 0.634077301699, 0.641778165875,
    0.656986598719, 0.664492646556, 0.671932245683, 0.679304652145,
    0.69384494493, 0.701011377236, 0.708107708988, 0.715133230559,
    0.728969040126, 0.735777944551, 0.742513271796, 0.749174348332,
    0.762271092361, 0.768705450192, 0.775062938118, 0.781342920396,
    0.793667863849, 0.799711592541, 0.805675350739, 0.811558542074,
    0.823080879012, 0.82871887239, 0.834273994572, 0.839745690049,
    0.850436620629, 0.855654786647, 0.860787387899, 0.86583391113,
    0.875666713593, 0.880452009553, 0.885149261046, 0.88975799835,
    0.898708095812, 0.903048560966, 0.907298722017, 0.911458153952,
    0.919503175829, 0.923387961276, 0.927180408695, 0.930880138847,
    0.937999976775, 0.941419372573, 0.944744627218, 0.947975408188,
    0.95415226628, 0.95709772572, 0.959947476186, 0.962701232705,
    0.967919672031, 0.970383833001, 0.972750956395, 0.975020805504,
    0.979267782686, 0.981244486064, 0.983123065812, 0.984903334072,
    0.988168233877, 0.989652538935, 0.991037879564, 0.992324117231,
    0.994598779111, 0.99558697586, 0.996475614741, 0.99726460689,
    0.998543345375, 0.999032963836, 0.999422679835, 0.999712454397,
    0.999992073306, 0.99998188969, 0.999871708718, 0.999661541409,
    0.99894134184, 0.998431381599, 0.997821579053, 0.99711199518,
    0.995393777258, 0.994385315028, 0.993277415096, 0.99207018825,
    0.989358246623, 0.987853803035, 0.98625057489, 0.984548722509,
    0.98084983562, 0.978853170999, 0.976758621876, 0.974566397704,
    0.969889810845, 0.967405915812, 0.964825280993, 0.96214816445,
    0.956505566652, 0.953540649651, 0.950480379379, 0.947325061862,
    0.94073055668, 0.93729202846, 0.933759771818, 0.930134139977,
    0.922604210239, 0.91870066533, 0.914705251119, 0.910618367146,
    0.902171833756, 0.897813028987, 0.893364443662, 0.888826522638,
    0.879484497531, 0.874681327643, 0.869790690351, 0.864813074715,
    0.854598908088, 0.849363378505, 0.844042913293, 0.838638044492,
    0.827577266442, 0.821922463262, 0.81618546852, 0.810366855911,
    0.798487112623, 0.792427169909, 0.786287985137, 0.780070172222,
    0.767401156867, 0.760951221319, 0.754425191282, 0.747823719355,
    0.734397097874, 0.727573290971, 0.720676727346, 0.713708096648,
    0.699557428603, 0.69237680631, 0.685126946913, 0.677808575392,
    0.662969230082, 0.655449740215, 0.64786470592, 0.640214885694,
    0.624723953754, 0.616884391121, 0.608983140563, 0.601020992198,
    0.584917192892, 0.576777152317, 0.568579434507, 0.560324859228,
    0.543648443666, 0.535228271011, 0.526754575975, 0.518228205921,
    0.501020856458, 0.49234159777, 0.483613105332, 0.474836251987,
    0.457140978035, 0.448224326941, 0.439262853787, 0.430257454714,
    0.412118485242, 0.402986728725, 0.39381467387, 0.384603237877,
    0.366065910862, 0.356741873558, 0.347382162364, 0.337987713243,
    0.319098362349, 0.309605349496, 0.300081376365, 0.290527395347,
    0.271333234114, 0.261694973299, 0.252030543204, 0.242340910266,
    0.2228899141, 0.213130495956, 0.203349764941, 0.193548699118,
    0.17388948538, 0.164033303371, 0.154160718167, 0.14427271702,
    0.124454423507, 0.114526112953, 0.104586349884, 0.0946361282662,
    0.0747082903895, 0.0647326668976, 0.0547505701928, 0.0447629984767,
    0.0247754254534, 0.0147774228867, 0.00477794259013, -0.00522201549675,
    -0.0252193651437, -0.0352147569854, -0.0452066273808, -0.0551939771511,
    -0.0751511204618, -0.0851189183045, -0.0950782043264, -0.105027982607,
    -0.124895037117, -0.134810326657, -0.144712135277, -0.154599472804,
    -0.174326781223, -0.184164779401, -0.193984361254, -0.203784544833,
    -0.223322799164, -0.233058916107, -0.242771727353, -0.252460261628,
    -0.271760626411, -0.281370526898, -0.290952290566, -0.300504959249,
    -0.319519193622, -0.328978857906, -0.338405624579, -0.347798550971,
    -0.366479129252, -0.375764913099, -0.385013120768, -0.394222827445,
    -0.412523057917, -0.421611751703, -0.430658284666, -0.439661752159,
    -0.457535893775, -0.4664047805, -0.475227027135, -0.484001751463,
    -0.501405128179, -0.510032040244, -0.518607949529, -0.527131998452};  