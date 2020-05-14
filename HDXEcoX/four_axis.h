
// #define MOVE_4AXIS_AREA 	0x6D763461  // [mv4a]
#define START_EVENT			0x73746576  // [stev]


typedef struct
{
	double radius;			// unit [mm]
	double distance;		// unit [mm]
	double alpha;			// unit [degree]
	double z;				// unit [mm]
	double zh;				// unint [mm]

	// double offset_D_Rate;
	// double offset_CP;		// unit [mm]
	// double offset_alpha;	// unit [degree]
} t_ObjectInfo;


typedef struct
{
	// LONG 	position_um;
	LONG 	init_CP_um;
	LONG	init_CW_um;
	LONG	init_CPZ_um;
	LONG	init_CWZ_um;

	double 	init_CW;
	double 	init_CP;
	double 	init_CPZ;
	double 	init_CWZ;
	
	double 	d_Theta;
	double 	d_Time;
	int 	index;
	LONG  	captureTime;
	LONG 	fpsCT;
}t_TrackingCO;

typedef struct
{
	LONG 	hl;
	LONG 	hr;
	LONG 	vt;
	LONG 	vb;
}t_Position_um;

typedef struct
{
	LONG 	hl_offsetPosition_um;
	LONG 	hr_offsetPosition_um;
	LONG 	vt_offsetPosition_um;
	LONG 	vb_offsetPosition_um;
} t_Info4Axis;

extern void TrackingObj(t_TrackingCO *);