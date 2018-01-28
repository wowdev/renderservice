parse_generic_args1:
	backgroundColor: hexcolor = 0
	portalsEnabled: bool = true
	fogEnabled: bool = true
	lightingEnabled: bool = true
	<no parameter>: bool = true
	ffxEnabled: bool = true
	<no parameter>: bool = true
	showDetailDoodads: bool = true
	showDoodads: bool = true
	showLiquid: bool = true
	showMapObjs: bool = true
	showParticles: bool = true
	showSky: bool = false
	showTerrain: bool = true
	showOccluders: bool = false
	showPortals: bool = false
	showGrid: bool = false
	overDraw: int = 0
		if overDraw > 0:
			showSky = false
			showTerrain = false
			showLiquid = false
			showMapObjs = false
			showDetailDoodads = false

parse_generic_args2:
	imgSize: int,int
	animSequence: int
	animVariation: int
	animTimer: int
	animRot: float 
	animShowWeapons: bool
	iaCrossFade: uint
	iaFramesPerSecond: uint
	irFrameCount: uint
	irRotateDeltaRadians: float
	nearClip: float
	farClip: float
		if nearClip >= farClip:
			farClip = nearClip + 1.0
	fov: float
	ortho: bool
	scale: float
	yaw: float
	pitch: float
	fitToScreen: bool
	trans: float,float,float

wmo_args:
	if request_path is numeric:
		fileName: string = "debug"
		fileID = atoi (request_path)
	else:
		path: string = ""
		filePath = path + "/" + request_path
	miniMap: bool = false
	chunkIndex: int = -1
	lightDir: float,float,float = 0.0,0.0,-1.0   # will be normalized
	lightCol: hexcolor = 0xff808080
	ambCol: hexcolor = 0xff202020
	if !miniMap:
		snapShot: bool = false

maps_args:
	mapID = atoi (request_path)
	coord: int,int = -1,-1
	if ! coord >= 0,0:
		pos: float,float,float = -1,-1,-1
		rect: float,float: 533.333,533.333
		vis_rect = pos.xy +- rect.xy / 2
	else:
		vis_rect = tile_to_worldcoord (coord)
		pos = vis_rect.center(), 3000
	lightDir: float,float,float = 0.0,0.0,-1.0  # will be normalized
	lightCol: hexcolor = 0xff808080
	ambCol: hexcolor = 0xff202020
	areaLightWater: bool = false
		
m2_args:
	if request_path is numeric:
		fileID = atoi (request_path)
	else:
		path: string = ""
		filePath = path + "/" + request_path
	enableGeosetList: bool = false
	geoset: string = "" # some weird list of uints, probably comma separated
	overDraw: int = 0
		
	
		
		