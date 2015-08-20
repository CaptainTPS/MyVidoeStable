
class CameraData{
public:
	double CameraPosition[3];
	double RotationAxisH[3];
	double RotationAxisV[3];
	double RotationAxisA[3];
	double Radialdistortion[2];
	double PixelSize[2];
	double ImageSize[2];
};

class OtherData{
public:
	double PPO[2];
	double FL;
	double HFofV;
	double RotationAxisH0[3];
	double RotationAxisV0[3];
};

class PointData{
public:
	double ImgCo[2];
	double Co3D[3]; 
};