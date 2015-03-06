from main import *
#Main method call
doVerif(0, "Teal"  , "Triangle"     ,1.0, 0, "Teal"  ,["I"],[1.0], 38.145014, -76.424933, 0, 0)
doVerif(0, "Teal"  , "Triangle"     ,1.0, 0, "Teal"  ,[""],[1.0], 38.144964, -76.424901, 0, 0)
doVerif(0, "Yellow", "Rectangle"    ,1.0, 0, "White" ,["N"],[1.0], 38.146039, -76.425706, 0, 0)
doVerif(0, "Yellow", "Rectangle"    ,1.0, 0, "White" ,[""] ,[1.0], 38.146057, -76.42567, 0, 0)
doVerif(0, "Black" ,""              ,1.0, 0, "Black" ,[""] ,[1.0], 38.146044, -76.425141, 0, 0)
doVerif(0, "Orange", "QuarterCircle",1.0, 0, "Teal"  ,["B"],[1.0], 38.145117, -76.425853, 0, 0)
doVerif(0, "Teal"  , "Triangle"     ,1.0, 0, "Teal"  ,[""] ,[1.0], 38.144993, -76.42495, 0, 0)
doVerif(0, "Black" ,""              ,1.0, 0, "Teal"  ,[""] ,[1.0], 38.14666, -76.42628, 0, 0)
doVerif(0, "Black" , "Semicircle"   ,1.0, 0, "Black" ,["O"],[1.0], 38.146456, -76.426586, 0, 0)
doVerif(0, "Teal"  ,""              ,1.0, 0, "Yellow",[""] ,[1.0], 38.144477, -76.427055, 0, 0)
doVerif(0, "Olive" ,""              ,1.0, 0, "Black" ,[""] ,[1.0], 38.144797, -76.426697, 0, 0)
doVerif(0, "Teal"  ,""              ,1.0, 0, "Yellow",[""] ,[1.0], 38.144493, -76.427016, 0, 0)

sendJudges()

os.remove("targets.p")