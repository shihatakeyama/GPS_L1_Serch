// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// GL_a.cpp
//
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#include "stdafx.h"

#include "resource.h"
#include "SignalProcDlgDlg.h"

#include "L1PrnTap.h"

#pragma warning(disable : 4996)


extern CGpsL1SearchDlg *gMainWind;

const char *sSDKsample = "L1 Signal Simulation";

#define MAX_EPSILON 0.10f
#define THRESHOLD   0.15f
#define REFRESH_DELAY     10 //ms

////////////////////////////////////////////////////////////////////////////////
// constants
unsigned int windowW = 512, windowH = 512;

unsigned int meshSizeW = 2048;
unsigned int meshSizeH = 32;
unsigned int meshSizeWNow = 0;
unsigned int meshSizeHNow = 0;


// OpenGL vertex buffers
GLuint posVertexBuffer;
GLuint heightVertexBuffer, slopeVertexBuffer;
struct cudaGraphicsResource *cuda_heightVB_resource=nullptr, *cuda_slopeVB_resource=nullptr; // handles OpenGL-CUDA �֘A�Â�	//	*cuda_posVB_resource, 

GLuint indexBuffer;
GLuint shaderProg;
char *vertShaderPath = 0, *fragShaderPath = 0;

// mouse controls
int mouseOldX, mouseOldY;
int mouseButtons = 0;

float translateX = -11.10f, translateY = 0.70f, translateZ = -36.0f;	// �����\��
float rotateX = 25.0f, rotateY = 99.0f;

float stretchX=10.0f,stretchY=5.0f;	// �c���̔䗦 CA�F�L�����A

bool animate = false;
bool drawPoints = false;
bool wireFrame = false;
bool g_hasDouble = false;
bool drawTilt = true;

//--float2 *d_slope = 0;	// CUDA

// pointers to device object
float *g_hptr = NULL;
float2 *g_sptr = NULL;

StopWatchInterface *timer = NULL;
float animTime = 0.0f;
float prevTime = 0.0f;
float animationRate = -0.001f;

int hMainWin = 0;

// �T�[�`����
uint16_t gL1Prn=0;
uint32_t gCarrMax=0,gCarrMin=0;
uint16_t gCodeMin=0,gCodeMax=1023;
uint32_t gCodePhaseMin=0,gCodePhaseMax=0xFFFFFFFF;
//	corrparam.fromCenterFreq(m_SampleClk	,m_CarrFreqCenter ,m_CarrStepWidth ,meshSizeH/2);

#if 1
extern "C"
void cudaUpdateHeightmapKernel(float  *d_heightMap,
                               float2 *d_ht,
                               unsigned int width,
                               unsigned int height,
                               bool autoTest);
#endif
extern "C"
void cudaCalculateSlopeKernel(float *h, float2 *slopeOut,
                              unsigned int width, unsigned int height ,float Sensitivity);

////////////////////////////////////////////////////////////////////////////////
// forward declarations
//void runAutoTest(int argc, char **argv);
void runGraphicsTest(int argc, char **argv);

// GL functionality
bool initGL(int *argc, char **argv);
void cleanup();
void createVBO(GLuint *vbo, int size);
void deleteVBO(GLuint *vbo);
void createMeshIndexBuffer(GLuint *id, int w, int h);
void createMeshPositionVBO(GLuint *id, int w, int h);
GLuint loadGLSLProgram(const char *vertFileName, const char *fragFileName);

// rendering callbacks
void display();
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void reshape(int w, int h);
void timerEvent(int value);

// Cuda functionality
void runCudaL1();

// �����̐ݒ�p
GLfloat red[] = { 1.0f, 0.4f, 0.3f, 1.0f };


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Numerator / Denominator(0x100000000��1.0�Ƃ���)
// �߂�l��32�̔{����
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
uint32_t division_a(uint32_t Numerator ,uint32_t Denominator)
{
	uint64_t ret = (uint64_t)Numerator<<32 / Denominator;
	
	return (uint32_t)((ret + (MAX_THREADS - 1)) / MAX_THREADS * MAX_THREADS);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// VBO�쐬
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void createMyVbo()
{
    int outputSize =  meshSizeW * meshSizeH * sizeof(float2);

    // create vertex buffers and register with CUDA
    createVBO(&heightVertexBuffer, meshSizeW * meshSizeH * sizeof(float));
//    createVBO(&heightVertexBuffer, meshSizeW * meshSizeH * sizeof(float));
	// OpenGL�̃o�b�t�@��Cuda�Ƌ��L����ݒ�
    checkCudaErrors(cudaGraphicsGLRegisterBuffer(&cuda_heightVB_resource, heightVertexBuffer, cudaGraphicsMapFlagsWriteDiscard));

    createVBO(&slopeVertexBuffer, outputSize);
    checkCudaErrors(cudaGraphicsGLRegisterBuffer(&cuda_slopeVB_resource, slopeVertexBuffer, cudaGraphicsMapFlagsWriteDiscard));

    // create vertex and index buffer for mesh
    createMeshPositionVBO(&posVertexBuffer, meshSizeW, meshSizeH);
    createMeshIndexBuffer(&indexBuffer, meshSizeW, meshSizeH);

	meshSizeWNow = meshSizeW;
	meshSizeHNow = meshSizeH;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//  VBO���ꂢ�����ς�
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void cleanupMyVbo()
{
	// check for error
    GLenum gl_error;

	if(cuda_heightVB_resource != nullptr){
		gl_error = glGetError();
		deleteVBO(&indexBuffer);
		deleteVBO(&posVertexBuffer);
		cudaGraphicsUnregisterResource(cuda_slopeVB_resource);
		deleteVBO(&slopeVertexBuffer);
		cudaGraphicsUnregisterResource(cuda_heightVB_resource);
		deleteVBO(&heightVertexBuffer);
		gl_error = glGetError();
	}

	meshSizeWNow = 0;
	meshSizeHNow = 0;
}

////////////////////////////////////////////////////////////////////////////////
//! Run test
////////////////////////////////////////////////////////////////////////////////
void runGraphicsTest(int argc, char **argv)
{
#if defined(__linux__)
    setenv ("DISPLAY", ":0", 0);
#endif
    
	try{
		initGL(&argc, argv);
	}catch(std::exception e){
		AfxGetApp()->GetMainWnd()->MessageBox(CString(e.what()));
		return;
	}

    findCudaDevice(argc, (const char **)argv);

    // allocate memory
//--    int spectrumSize = spectrumW*spectrumH*sizeof(float2);
//--    checkCudaErrors(cudaMalloc((void **)&d_slope, outputSize));

    sdkCreateTimer(&timer);
    sdkStartTimer(&timer);
    prevTime = sdkGetTimerValue(&timer);

#if 0
    int outputSize =  meshSizeW * meshSizeH * sizeof(float2);

    // create vertex buffers and register with CUDA
    createVBO(&heightVertexBuffer, meshSizeW * meshSizeH * sizeof(float));
    checkCudaErrors(cudaGraphicsGLRegisterBuffer(&cuda_heightVB_resource, heightVertexBuffer, cudaGraphicsMapFlagsWriteDiscard));

    createVBO(&slopeVertexBuffer, outputSize);
    checkCudaErrors(cudaGraphicsGLRegisterBuffer(&cuda_slopeVB_resource, slopeVertexBuffer, cudaGraphicsMapFlagsWriteDiscard));

    // create vertex and index buffer for mesh
    createMeshPositionVBO(&posVertexBuffer, meshSizeW, meshSizeH);
    createMeshIndexBuffer(&indexBuffer, meshSizeW, meshSizeH);
#else
	createMyVbo();
#endif

//    runCuda();
//	runCudaL1();

    // register callbacks
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutReshapeFunc(reshape);
    glutTimerFunc(REFRESH_DELAY, timerEvent, 0);

	// OPENGL�I�����AglutMainLoop()���Ăяo�����ɖ߂�悤�ɂ���B
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);

//	glEnable(GL_LIGHT0);				   // 0�Ԗڂ̌����_��
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, red); // �g�U���E��

//glDisable(GL_LIGHT0);				   // 0�Ԗڂ̌����_��
//glDisable(GL_LIGHTING);

    // start rendering mainloop
    glutMainLoop();

	cleanup();
}

int32_t maxChipPos(const float *InData ,size_t ChipLen);
int32_t maxChipPos(const int32_t *InData ,size_t ChipLen);

///--int GPU_L1Accumlevel(float *Devout ,const uint8_t *InData ,size_t DataLen);
int GPU_L1AccumlevelCh1(float *Devout ,float *CorrOut ,int32_t width ,int32_t height ,size_t AccumLen ,GPU_L1_FreqPhase &CorrParam);
static float sOutDataf[MAX_THREADS*64*40];

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// GL�ŕ\������A���C�h�w�C�g�̔䗦�𐧌����܂��B
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void match22(float32_t &out_x ,float32_t &out_y ,int32_t in_x ,int32_t in_y ,const float32_t Min ,const float32_t Max)
{
	if(in_x < in_y){
		out_x = (float32_t)in_x / (float32_t)in_y * Max;
		if(out_x < Min) { out_x = Min; }
		else if(out_x > Max) { out_x = Max; }
		out_y = Max;
	}else{
		out_x = Max;
		out_y = (float32_t)in_y / (float32_t)in_x * Max;
		if(out_y < Min) { out_y = Min; }
		else if(out_y > Max) { out_y = Max; }
	}
}
void addL1ChipAndPhase(uint32_t &AnsChip ,uint32_t AnsPhase ,uint32_t Arg1Chip ,uint32_t Arg1Phse ,uint32_t Arg2Chip ,uint32_t Arg2Phse)
{
	uint32_t phase = Arg1Phse + Arg2Phse;
	uint32_t chip=0;

	if(phase < Arg1Phse){
		chip++;
	}

	chip = Arg1Chip + Arg2Chip + chip;

	AnsChip = AnsChip % 1023;
	AnsPhase = chip;
}

int32_t gCodePhaseReso = 2;		// ���͂���ChipPhase����\
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// L1�T�[�`�����Z�o�A�v�Z�A���x���W�v
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int32_t CGpsL1SearchDlg::GTS_AccumSum()
{
    size_t num_bytes;
	GPU_L1_FreqPhase corrparam;

	corrparam.Sv = m_SvNo;	//L1StaticPrn::prnToSv(m_PrnNo);

	gL1Prn		= L1StaticPrn::svToPrn(m_SvNo);

	if(m_SearchMode == 0){
		// �T�[�`
		meshSizeW	= ((MAX_L1_CA_CODE * m_CodePhaseReso) + (MAX_THREADS - 1)) / MAX_THREADS * MAX_THREADS;	// MAX_THREADS �̔{����
		meshSizeH	= m_CarrStepN;

		gCodePhaseReso = m_CodePhaseReso;
		corrparam.fromCenterFreq(m_SampleClk	,m_CarrFreqCenter ,m_CarrStepWidth ,meshSizeH/2);
		corrparam.fromBaseCodePhase(m_SampleClk ,0 ,0 ,(uint32_t)(0x0000000100000000l/m_CodePhaseReso) ,0);

		gCarrMax	= m_CarrFreqCenter - (m_CarrStepWidth * (meshSizeH/2));		// �T�[�`�p�̎Z�o
		gCarrMin	= gCarrMax + (m_CarrStepWidth * meshSizeH);
		gCodeMin	= 0	,gCodeMax	= MAX_L1_CA_CODE;
		gCodePhaseMin=0	,gCodePhaseMax=0xFFFFFFFF;

	}else{
		// �t�@�C��
		meshSizeW	= m_FineCodeStepN;
		meshSizeH	= m_FineCarrStepN;

		gCodePhaseReso = m_FineCodePhaseReso;
		corrparam.fromCenterFreq(m_SampleClk	,m_DetectCarrFreq	,m_FineCarrStepWidth ,m_FineCarrStepN/2);
		corrparam.fromCenterCaPhase(m_SampleClk	,m_DetectCodeChip	,m_DetectCodePhasei	,(uint32_t)(0x0000000100000000l/m_FineCodePhaseReso)	,m_FineCodeStepN/2);

		gCarrMax	= m_DetectCarrFreq - (m_FineCarrStepWidth * (meshSizeH/2));		// �T�[�`�p�̎Z�o
		gCarrMin	= gCarrMax + (m_FineCarrStepWidth * meshSizeH);
		corrparam.getCodePhase(gCodeMin ,gCodePhaseMin ,0);
		corrparam.getCodePhase(gCodeMax ,gCodePhaseMax ,m_FineCarrStepN);
	}

	match22(stretchX ,stretchY ,meshSizeW ,meshSizeH ,5.0f ,10.0f);

	// Vbo���Ȃ���
	if((meshSizeWNow != meshSizeW) || (meshSizeHNow != meshSizeH)){
		cleanupMyVbo();
		createMyVbo();
	}

    // update heightmap values in vertex buffer
    checkCudaErrors(cudaGraphicsMapResources(1, &cuda_heightVB_resource, 0));
    checkCudaErrors(cudaGraphicsResourceGetMappedPointer((void **)&g_hptr, &num_bytes, cuda_heightVB_resource));

	int32_t ack;
	int32_t x,y;
	size_t procsize = (size_t)m_SampleClk * m_AccumPeriod / 1000;// 16.368e6*2/1000;		// �ώZ��

	GPU_L1AccumlevelCh1(g_hptr ,sOutDataf ,meshSizeW ,meshSizeH ,procsize ,corrparam);	// 2

//	cudaUpdateHeightmapKernel(Hptr ,nullptr ,meshSizeW ,meshSizeH ,true);

	L1TrackingCh &tack = gL1TrackingCh[corrparam.Sv];

	ack = maxChipPos(sOutDataf ,meshSizeW * meshSizeH);
	if(ack >= 0){
		x	= ack % meshSizeW;
		y	= ack / meshSizeW;

		corrparam.getCodePhase(tack.CodeChip ,tack.CodePhase ,x);

//		tack.CodeChip		= x/gCodePhaseReso;
//		tack.CodePhase		= (x%gCodePhaseReso) * 0x0000000100000000l / gCodePhaseReso;
		if(m_SearchMode == 0){
			tack.CarrDcoAcquire	= m_CarrFreqCenter + (m_CarrStepWidth * (y - (int32_t)meshSizeH/2));	// �T�[�`�p�̎Z�o
		}else{
			tack.CarrDcoAcquire	= m_DetectCarrFreq + (m_FineCarrStepWidth * (y - (int32_t)meshSizeH/2));
		}
		tack.Sv = corrparam.Sv;		// PRN�֕ύX���ȁH�H
		tack.CdLI = (uint32_t)(sOutDataf[ack]);

		m_DetectLevel		= sOutDataf[ack] * 2048*16;
		m_DetectCarrFreq	= tack.CarrDcoAcquire;
		m_DetectCodeChip	= tack.CodeChip;
		uint32ToDump16(m_DetectCodePhase ,tack.CodePhase ,8);

	}

	AfxGetApp()->GetMainWnd()->SendMessage(WM_COM_RECV_DATA, 0, 0);


	checkCudaErrors(cudaGraphicsUnmapResources(1, &cuda_heightVB_resource, 0));

    // calculate slope for shading
    checkCudaErrors(cudaGraphicsMapResources(1, &cuda_slopeVB_resource, 0));
    checkCudaErrors(cudaGraphicsResourceGetMappedPointer((void **)&g_sptr, &num_bytes, cuda_slopeVB_resource));

	if(m_SearchMode == 0){
		// �T�[�`
		cudaCalculateSlopeKernel(g_hptr, g_sptr, meshSizeW, meshSizeH ,0.002f);
	}else{
		// �t�@�C��
		cudaCalculateSlopeKernel(g_hptr, g_sptr, meshSizeW, meshSizeH ,0.01f);	
	}
    checkCudaErrors(cudaGraphicsUnmapResources(1, &cuda_slopeVB_resource, 0));

	return 0;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// CUDA�AGL�A���C�� �̐ڍ���
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void runCudaL1()
{

	gMainWind->GTS_AccumSum();


}
void Square2D(int x1,int y1,int x2, int y2,float size){
 glColor3f(0.5f, 0.5f, 0.5f);
 glLineWidth(size);
 glBegin(GL_LINE_LOOP);
 glVertex2i(x1,y1);
 glVertex2i(x2,y1);
 glVertex2i(x2,y2);
 glVertex2i(x1,y2);
 glEnd();
}

////////////////////////////////////////////////////////////////////////////////
// ������`��
// @param[in] str ������
// @param[in] w,h �E�B���h�E�T�C�Y
// @param[in] x0,y0 ������̈ʒu(���㌴�_�̃X�N���[�����W�n,������̍��������̈ʒu�ɂȂ�)
////////////////////////////////////////////////////////////////////////////////
static void DrawString(string str, float w, float h, float x0, float y0)
{

    glDisable(GL_LIGHTING);
    // ���s���e�ɂ���
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
//    gluOrtho2D(0.0f, w, h, 0.0f);		// gluOrtho2D(left, right, bottom, top)  // ���W�n�ݒ�
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    // ��ʏ�Ƀe�L�X�g�`��
    glRasterPos2f(x0, y0);
    int size = (int)str.size();
    for(int i = 0; i < size; ++i){
        char ic = str[i];
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ic);
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
inline void glPrint(float x, float y, const char *s, void *font)
{
    glRasterPos2f((GLfloat)x, (GLfloat)y);
    int len = (int) strlen(s);

    for (int i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, s[i]);
    }
}
static GLfloat lightPosition[4] = { -0.0f, -0.0f, 0.0f, 1.0f }; //�����̈ʒu


////////////////////////////////////////////////////////////////////////////////
//! Display callback
////////////////////////////////////////////////////////////////////////////////
void display()
{
	std::string str;


    // run CUDA kernel to generate vertex positions
    if (animate){
    }
	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(gGLThread.m_PramTrig == 1){
		gGLThread.m_PramTrig = 0;
		runCudaL1();
	}

    // set view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


//glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
//glDisable(GL_LIGHT0);				   // 0�Ԗڂ̌����_��
//glDisable(GL_LIGHTING);
//glDisable(GL_DEPTH_TEST);
//glLightfv(GL_LIGHT0, GL_DIFFUSE, red); // �g�U���E��

#if 0
static GLfloat lightPosition[4] = { 0, 0.0, 0.0, 1.0 }; //�����̈ʒu
static GLfloat lightDiffuse[3]  = { 1.0,   1.0, 1.0  }; //�g�U��
static GLfloat lightAmbient[3]  = { 0.25, 0.25, 0.25 }; //����
static GLfloat lightSpecular[3] = { 1.0,   1.0, 1.0  }; //���ʌ�
GLfloat spotDirrection[] = { -0.5, 0.0, -1.0 };			//�X�|�b�g���C�g�����������

glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);  
glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDiffuse);
glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmbient);
glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
glLightf( GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.01);
glLightfv( GL_LIGHT0, GL_SPOT_DIRECTION, spotDirrection ); //�X�|�b�g���C�g�̌���������i�f�t�H���g (0,0,-1.0)�j
glLightf( GL_LIGHT0, GL_SPOT_CUTOFF,  30.0 );// �X�|�b�g���C�g�̍i��i�f�t�H���g 180.0�j
glLightf( GL_LIGHT0, GL_SPOT_EXPONENT, 0.1 );// �X�|�b�g���C�g�̒��S����̌����̓x�����i�f�t�H���g 0�j
#endif

#if 0
    glTranslatef(translateX, translateY ,translateZ);	// �����𕽍s�ړ� �ړ����������ɐݒ�
	glRotatef(rotateX, 1.0f, 0.0f, 0.0f);				// ��������]
    glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
#else
	{
		float sx=stretchX;
		float sy=0;
		float sz=stretchY;
		glTranslatef(translateX+sx, translateY+sy, translateZ+sz);	// �����𕽍s�ړ�
		glRotatef(rotateX, 1.0f, 0.0f, 0.0f);				// ��������]
		glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
		glTranslatef(-sx, -sy, -sz);
	}
#endif

	// ������`��
	{
		str = "PRN=" + std::to_string(gL1Prn);
		DrawString(str, 0, 0, -1.0f, +0.90f);

//		str = std::to_string(1023);
//		glPrint(20, 2, str.c_str(), GLUT_BITMAP_9_BY_15);
	}

	if(drawTilt){
		str = "translateX=" + std::to_string(translateX);
		DrawString(str, 0.0f, 0.0f, -0.75f, +0.90f);
		str = "translateY=" + std::to_string(translateY);
		DrawString(str, 00, 0, -0.75f, +0.85f);
		str = "translateZ=" + std::to_string(translateZ);
		DrawString(str, 00, 00, -0.75f, +0.80f);

		str = "rotateX=" + std::to_string(rotateX);
		DrawString(str, 0, 0, -0.75f, +0.75f);
		str = "rotateY=" + std::to_string(rotateY);
		DrawString(str, 0, 0, -0.75f, +0.70f);
	}

#if 1
    // render from the vbo
    glBindBuffer(GL_ARRAY_BUFFER, posVertexBuffer);
    glVertexPointer(4, GL_FLOAT, 0, 0);
    glEnableClientState(GL_VERTEX_ARRAY);
#endif

#if 1
    glBindBuffer(GL_ARRAY_BUFFER, heightVertexBuffer);
    glClientActiveTexture(GL_TEXTURE0);
    glTexCoordPointer(1, GL_FLOAT, 0, 0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#endif

#if 1
    glBindBuffer(GL_ARRAY_BUFFER, slopeVertexBuffer);
    glClientActiveTexture(GL_TEXTURE1);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#endif



	// �n�_�̋�
    glPushMatrix();
    glTranslatef(-1.0f, -0.0f, -1.0f);
    glColor3f(1.0, 1.0, 0.0);
    glutSolidSphere(0.05f, 10, 10);	// ���a,

    glRotatef(90.0, 3.0, 0.0, 0.0);

	str = std::to_string(gCarrMin) + "Hz";
	glPrint(-0.5f, 0.0f, str.c_str(), GLUT_BITMAP_9_BY_15);
	str = std::to_string(gCarrMax) + "Hz";
	glPrint(-0.5f, stretchY*2-1.0f, str.c_str(), GLUT_BITMAP_9_BY_15);

	str = std::to_string(gCodeMin) + "Chip";
	glPrint(1.0f, -0.5f, str.c_str(), GLUT_BITMAP_9_BY_15);
	str = std::to_string(gCodeMax) + "Chip";
	glPrint(stretchX*2+1.0f ,-1.0f , str.c_str(), GLUT_BITMAP_9_BY_15);

	Square2D(0 ,0 ,stretchX*2 ,stretchY*2 ,1.0f);	// ��̒����`

//	glutWireCube(4.0f);				// ������

    glPopMatrix();


    glUseProgram(shaderProg);

    // Set default uniform variables parameters for the vertex shader
    GLuint uniHeightScale, uniChopiness, uniSize;

    uniHeightScale = glGetUniformLocation(shaderProg, "heightScale");
    glUniform1f(uniHeightScale, 0.5f);

    uniChopiness   = glGetUniformLocation(shaderProg, "chopiness");
    glUniform1f(uniChopiness, 1.0f * 2);

    uniSize        = glGetUniformLocation(shaderProg, "size");
    glUniform2f(uniSize, (float) meshSizeW, (float) meshSizeH);

    // Set default uniform variables parameters for the pixel shader
    GLuint uniDeepColor, uniShallowColor, uniSkyColor, uniLightDir;

#if 1
	uniDeepColor = glGetUniformLocation(shaderProg, "deepColor");
    glUniform4f(uniDeepColor, 0.0f, 0.1f, 0.4f, 1.0f);

    uniShallowColor = glGetUniformLocation(shaderProg, "shallowColor");
    glUniform4f(uniShallowColor, 0.1f, 0.3f, 0.3f, 1.0f);

    uniSkyColor = glGetUniformLocation(shaderProg, "skyColor");
    glUniform4f(uniSkyColor, 1.0f, 1.0f, 1.0f, 1.0f);

    uniLightDir = glGetUniformLocation(shaderProg, "lightDir");
    glUniform3f(uniLightDir, 0.0f, 1.0f, 0.0f);
    // end of uniform settings
#endif

    glColor3f(1.0f, 1.0f, 1.0f);

    if (drawPoints){
        glDrawArrays(GL_POINTS, 0, meshSizeW * meshSizeH);
    }else{
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

        glPolygonMode(GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL);
        glDrawElements(GL_TRIANGLE_STRIP, ((meshSizeW*2)+2)*(meshSizeH-1), GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glClientActiveTexture(GL_TEXTURE0);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE1);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glUseProgram(0);

    glutSwapBuffers();


	//computeFPS();
}

void timerEvent(int value)
{
    float time = sdkGetTimerValue(&timer);

    if (animate){
        animTime += (time - prevTime) * animationRate;
    }

	if(gGLThread.m_PramTrig == 1){
	    glutPostRedisplay();	// �`��˗�
	}

	prevTime = time;

    glutTimerFunc(REFRESH_DELAY, timerEvent, 0);
}

void cleanup()
{
    sdkDeleteTimer(&timer);

#if 0
//    checkCudaErrors();
	cudaGraphicsUnregisterResource(cuda_heightVB_resource);
	cudaGraphicsUnregisterResource(cuda_slopeVB_resource);
//    checkCudaErrors();

    deleteVBO(&posVertexBuffer);
    deleteVBO(&heightVertexBuffer);
    deleteVBO(&slopeVertexBuffer);
//--	cudaFree(d_slope);
	//    checkCudaErrors();
#else
	cleanupMyVbo();
#endif
}

////////////////////////////////////////////////////////////////////////////////
//! Keyboard events handler
////////////////////////////////////////////////////////////////////////////////
void keyboard(unsigned char key, int /*x*/, int /*y*/)
{
	int ack;

	ack = glutGetModifiers();
	
//	GLUT_ACTIVE_SHIFT;

    switch (key){
        case (27):	// ESC
 //           cleanup();
//            exit(EXIT_SUCCESS);
			glutDestroyWindow(hMainWin);
			break;
        case 'w':
            wireFrame = !wireFrame;
			glutPostRedisplay();	// �`��˗�
            break;

        case 'p':
            drawPoints = !drawPoints;
			glutPostRedisplay();
            break;

        case ' ':
            animate = !animate;
		    glutPostRedisplay();
            break;

		case 't':
			drawTilt = !drawTilt;	// �����ʒu�p�x�\�� ON/OFF
		    glutPostRedisplay();
			break;
    }
}

////////////////////////////////////////////////////////////////////////////////
//! Mouse event handlers
////////////////////////////////////////////////////////////////////////////////
void mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN){
        mouseButtons |= 1<<button;
    }
    else if (state == GLUT_UP){
        mouseButtons = 0;
    }

    mouseOldX = x;
    mouseOldY = y;
    glutPostRedisplay();	// �`��˗�
}
float sigmoid(float x)
{
  return 0.2f / ((1.0f+ exp(x)) * 2);///1.0f +
//  return exp(x);
}

void motion(int x, int y)
{
	int ack;
	float ftemp;
    float dx, dy;
    dx = (float)(x - mouseOldX);
    dy = (float)(y - mouseOldY);

	ack = glutGetModifiers();
	switch(ack){
	case GLUT_ACTIVE_SHIFT:
//--		glTranslated((float)dx/64.0f, (float)dy/64.0f, 0.0);	// �ڂ𕽍s�ړ�
		break;
	default:
		if (mouseButtons == 1){			// ���{�^��
			rotateX += dy * 0.2f;
			rotateY += dx * 0.2f;
		}else if (mouseButtons == 2){	// �z�C���[�{�^��
			ftemp = sigmoid(translateZ);
			translateX += dx * ftemp;	//0.01f;
			translateY -= dy * ftemp;	//0.01f;
		}else if (mouseButtons == 4){	// �E�{�^��
			ftemp = sigmoid(translateZ);
			translateZ += dy * ftemp + 0.00001f;
		}
		break;
	}

    mouseOldX = x;
    mouseOldY = y;

    glutPostRedisplay();	
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#if 1
	// ��ʔ�i�A�X�y�N�g��j���v�Z
	float aspect = 1.0f;//(float) 800 / 480;

	// gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
	gluPerspective(30.0, aspect, 1.0, 100.0);
//	glTranslated(0.0, 0.0, -5.0);	// �ڂ𕽍s�ړ�
#else
    gluPerspective(60.0, (double) w / (double) h, 0.1, 10.0);
#endif
    windowW = w;
    windowH = h;
}

////////////////////////////////////////////////////////////////////////////////
//! Initialize GL
////////////////////////////////////////////////////////////////////////////////
bool initGL(int *argc, char **argv)
{
    // Create GL context
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(windowW, windowH);
    hMainWin = glutCreateWindow(sSDKsample);

    vertShaderPath = sdkFindFilePath("ocean.vert", argv[0]);
    fragShaderPath = sdkFindFilePath("ocean.frag", argv[0]);

    if (vertShaderPath == NULL || fragShaderPath == NULL){
		throw std::runtime_error("Error unable to find GLSL vertex and fragment shaders!");
	}

    // initialize necessary OpenGL extensions
    if (! isGLVersionSupported(2,0)){
		throw std::runtime_error("ERROR: Support for necessary OpenGL extensions missing.");
    }

    if (!areGLExtensionsSupported("GL_ARB_vertex_buffer_object GL_ARB_pixel_buffer_object")){
        fprintf(stderr, "Error: failed to get minimal extensions for demo\n");
        fprintf(stderr, "This sample requires:\n");
        fprintf(stderr, "  OpenGL version 1.5\n");
        fprintf(stderr, "  GL_ARB_vertex_buffer_object\n");
        fprintf(stderr, "  GL_ARB_pixel_buffer_object\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    // default initialization
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);

    // load shader
    shaderProg = loadGLSLProgram(vertShaderPath, fragShaderPath);

    SDK_CHECK_ERROR_GL();

	free(vertShaderPath);
	free(fragShaderPath);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
//! Create VBO
////////////////////////////////////////////////////////////////////////////////
void createVBO(GLuint *vbo, int size)
{
    // create buffer object
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    SDK_CHECK_ERROR_GL();
}

////////////////////////////////////////////////////////////////////////////////
//! Delete VBO
////////////////////////////////////////////////////////////////////////////////
void deleteVBO(GLuint *vbo)
{
    glDeleteBuffers(1, vbo);
    *vbo = 0;
}

// create index buffer for rendering quad mesh
void createMeshIndexBuffer(GLuint *id, int w, int h)
{
    int size = ((w*2)+2)*(h-1)*sizeof(GLuint);

    // create index buffer
    glGenBuffers(1, id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, 0, GL_STATIC_DRAW);

    // fill with indices for rendering mesh as triangle strips
    GLuint *indices = (GLuint *) glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

    if (!indices){
        return;
    }

    for (int y=0; y<h-1; y++){
        for (int x=0; x<w; x++){
            *indices++ = y*w+x;
            *indices++ = (y+1)*w+x;
        }

        // start new strip with degenerate triangle
        *indices++ = (y+1)*w+(w-1);
        *indices++ = (y+1)*w;
    }

    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// create fixed vertex buffer to store mesh vertices
void createMeshPositionVBO(GLuint *id, int w, int h)
{
    createVBO(id, w*h*4*sizeof(float));

    glBindBuffer(GL_ARRAY_BUFFER, *id);
    float *pos = (float *) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    if (!pos){
        return;
    }

    for (int y=0; y<h; y++){
        for (int x=0; x<w; x++){
            float u = x*stretchX / (float)(w-1);
            float v = y*stretchY / (float)(h-1);
            *pos++ = u*2.0f-1.0f;	// �\���I�t�Z�b�g 
            *pos++ = 0.0f;
            *pos++ = v*2.0f-1.0f;
            *pos++ = 0.0f;
        }
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Attach shader to a program
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int attachShader(GLuint prg, GLenum type, const char *name)
{
    GLuint shader;
    FILE *fp;
    int size, compiled;
    char *src;

    fp = fopen(name, "rb");

    if (!fp){
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    src = (char *)malloc(size);

    fseek(fp, 0, SEEK_SET);
    fread(src, sizeof(char), size, fp);
    fclose(fp);

    shader = glCreateShader(type);
    glShaderSource(shader, 1, (const char **)&src, (const GLint *)&size);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, (GLint *)&compiled);

    if (!compiled){
        char log[2048];
        int len;

        glGetShaderInfoLog(shader, 2048, (GLsizei *)&len, log);
        printf("Info log: %s\n", log);
        glDeleteShader(shader);
        return 0;
    }

    free(src);

    glAttachShader(prg, shader);
    glDeleteShader(shader);

    return 1;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Create shader program from vertex shader and fragment shader files
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
GLuint loadGLSLProgram(const char *vertFileName, const char *fragFileName)
{
    GLint linked;
    GLuint program;

    program = glCreateProgram();

    if (!attachShader(program, GL_VERTEX_SHADER, vertFileName)){
        glDeleteProgram(program);
        fprintf(stderr, "Couldn't attach vertex shader from file %s\n", vertFileName);
        return 0;
    }

    if (!attachShader(program, GL_FRAGMENT_SHADER, fragFileName)){
        glDeleteProgram(program);
        fprintf(stderr, "Couldn't attach fragment shader from file %s\n", fragFileName);
        return 0;
    }

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (!linked){
        glDeleteProgram(program);
        char temp[256];
        glGetProgramInfoLog(program, 256, 0, temp);
        fprintf(stderr, "Failed to link program: %s\n", temp);
        return 0;
    }

    return program;
}


