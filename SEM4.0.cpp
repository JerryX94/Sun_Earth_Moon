// SEM4.0.cpp: 主项目文件。

#include "stdafx.h"
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//将星历计算程序以动态链接库的方式载入本程序
extern "C" void __stdcall ephemeris_de405(double RV[6], int year, int month, int day, int hour, int minute, double second, int targ, int observer); 

struct ephemeris
{
	double x, y, z, u, v, w; //分别代表天体在天球空间直角坐标系三坐标轴上的位置坐标和速度投影
};

const double rsun = 6.96e5, rearth = 6372.797, rmoon = 1737.53; //日地月平均半径，单位km
const double res = 1.496e8, rem = 384399.0; //日地、日月轨道半长轴，单位km
const double ksun = 80.0, kearth = 3000.0, kmoon = 4000.0, kes = 2.0, kem = 160.0; //glut绘图缩放系数（按正常比例什么都看不见……）
const int width = 800, height = 600; //窗口大小
struct ephemeris moon, sun; //存储当前天体数据
double persp[3] = {60.0, 1.0, 20.0}; //视角
double lookat[9] = {0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0}; //观察点
static int n = -1, t = 0, s = 0;
int year ,month, day, hour, minute, yeart ,montht, dayt, hourt, minutet; //时间
float second, secondt; //时间
int step; //时间间隔步长代号

void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
}

void resharp(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(persp[0], (GLfloat)w/(GLfloat)h, persp[1], persp[2]);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(lookat[0], lookat[1], lookat[2], lookat[3], lookat[4], lookat[5], lookat[6], lookat[7], lookat[8]);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	resharp(width, height);

	GLfloat env_light_position[] = {1.0f, 1.0f, 1.0f, 0.0f};
	GLfloat env_light_ambient[] = {0.7f, 0.7f, 0.7f, 1.0f};
	GLfloat env_light_diffuse[] = {0.2f, 0.2f, 0.2f, 1.0f};
	GLfloat env_light_specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, env_light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, env_light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, env_light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, env_light_specular);
	
	GLfloat earth_mat_ambient[] = {0.0f, 0.3f, 0.7f, 1.0f};
	GLfloat earth_mat_diffuse[] = {0.0f, 0.3f, 0.7f, 1.0f};
	GLfloat earth_mat_specular[] = {0.9f, 0.9f, 0.9f, 1.0f};
	GLfloat earth_mat_emission[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat earth_mat_shininess = 20.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat_specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, earth_mat_emission);
	glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
	glutSolidSphere(kearth*rearth/res, 30, 30);
	
	GLfloat moon_mat_ambient[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat moon_mat_diffuse[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat moon_mat_specular[] = {0.6f, 0.6f, 0.6f, 1.0f};
	GLfloat moon_mat_emission[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat moon_mat_shininess = 30.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, moon_mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, moon_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, moon_mat_specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, moon_mat_emission);
	glMaterialf(GL_FRONT, GL_SHININESS, moon_mat_shininess);
	glTranslatef(kem*moon.x/res, kem*moon.y/res, kem*moon.z/res);
	glutSolidSphere(kmoon*rmoon/res, 16, 16);
	
	GLfloat sun_mat_ambient[] = {1.0f, 0.9f, 0.0f, 1.0f};
	GLfloat sun_mat_diffuse[] = {1.0f, 0.9f, 0.0f, 1.0f};
	GLfloat sun_mat_specular[] = {1.0f, 0.9f, 0.0f, 1.0f};
	GLfloat sun_mat_emission[] = {0.1f, 0.1f, 0.1f, 1.0f};
	GLfloat sun_mat_shininess = 10.0f;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, sun_mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, sun_mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, sun_mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, sun_mat_emission);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, sun_mat_shininess);
	glTranslatef((kes*sun.x - kem*moon.x)/res, (kes*sun.y - kem*moon.y)/res, (kes*sun.z - kem*moon.z)/res);
	glutSolidSphere(ksun*rsun/res, 50, 50);
	
	GLfloat sun_light_position[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat sun_light_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat sun_light_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
	GLfloat sun_light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glLightfv(GL_LIGHT1, GL_POSITION, sun_light_position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, sun_light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, sun_light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, sun_light_specular);
	
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	
	printf("%4d-%2d-%2d, %2d:%2d:%5.2f\n", year, month, day, hour, minute, second);
	glPopMatrix();
	glutSwapBuffers();
	glFlush();
}

//使用动态链接库计算固定时刻天体数据
void inputcele(ephemeris *cele, int targ)
{
	double RV[6];

	ephemeris_de405(RV, year, month, day, hour, minute, second, targ, 3);
	(*cele).x = RV[0]; (*cele).y = RV[1]; (*cele).z = RV[2];
	(*cele).u = RV[3]; (*cele).v = RV[4]; (*cele).w = RV[5];
}

//时间切换
void timealter(int *ye, int *mo, int *da, int *ho, int *mi, float *se, int code)
{
	int k = (code>0)? 1: -1;
	int moo, run;

	//判断切换时长
	switch (abs(code))
	{
	case 1:
		*ye += k;
		break;
	case 2:
		*mo += k;
		break;
	case 3:
		*da += k;
		break;
	case 4:
		*ho += k;
		break;
	case 5:
		*mi += k;
		break;
	case 6:
		*se += 1.0*k;
		break;
	default:
		break;
	}

	//秒调整
	if (*se >= 60.0)
	{
		*se -= 60.0;
		*mi += 1;
	}
	if (*se < 0.0)
	{
		*se += 60.0;
		*mi -= 1;
	}
	//分调整
	if (*mi >= 60)
	{
		*mi -= 60;
		*ho += 1;
	}
	if (*mi < 0)
	{
		*mi += 60;
		*ho -= 1;
	}
	//时调整
	if (*ho >= 24)
	{
		*ho -= 24;
		*da += 1;
	}
	if (*ho < 0)
	{
		*ho += 24;
		*da -= 1;
	}

	//判断闰年
	if (*ye % 400 == 0)
	{
		run = 29;
	}
	else if (*ye % 100 == 0)
	{
		run = 28;
	}
	else if (*ye % 4 == 0)
	{
		run = 29;
	}
	else
	{
		run = 28;
	}
	
	//判断大小月
	if (*mo == 2)
	{
		moo = 3;
	}
	else if ((*mo == 4)||(*mo == 6)||(*mo == 9)||(*mo == 11))
	{
		moo = 2;
	}
	else
	{
		moo = 1;
	}

	//日调整
	switch (moo)
		{
		case 3:
			if (*da >= run + 1)
			{
				*da -= run;
				*mo += 1;
			}
			if (*da <= 0)
			{
				*da += 31;
				*mo -= 1;
			}
			break;
		case 2:
			if (*da >= 31)
			{
				*da -= 30;
				*mo += 1;
			}
			if (*da <= 0)
			{
				*da += 31;
				*mo -= 1;
			}
			break;
		default:
			if (*da >= 32)
			{
				*da -= 31;
				*mo += 1;
			}
			if (*da <= 0)
			{
				switch (*mo)
				{
				case 1:
					*da += 31;
					*mo -= 1;
					break;
				case 3:
					*da += run;
					*mo -= 1;
					break;
				case 8:
					*da += 31;
					*mo -= 1;
					break;
				default:
					*da += 30;
					*mo -= 1;
					break;
				}
			}
			break;
		}

	//月调整
	if (*mo >= 13)
	{
		*mo -= 12;
		*ye += 1;
	}
	if (*mo <= 0)
	{
		*mo += 12;
		*ye -= 1;
	}
}

//键盘输入消息处理
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'y':
		timealter(&year, &month, &day, &hour, &minute, &second, 1);
		break;
	case 'Y':
		timealter(&year, &month, &day, &hour, &minute, &second, -1);
		break;
	case 'm':
		timealter(&year, &month, &day, &hour, &minute, &second, 2);
		break;
	case 'M':
		timealter(&year, &month, &day, &hour, &minute, &second, -2);
		break;
	case 'd':
		timealter(&year, &month, &day, &hour, &minute, &second, 3);
		break;
	case 'D':
		timealter(&year, &month, &day, &hour, &minute, &second, -3);
		break;
	case 'h':
		timealter(&year, &month, &day, &hour, &minute, &second, 4);
		break;
	case 'H':
		timealter(&year, &month, &day, &hour, &minute, &second, -4);
		break;
	case 'f':
		timealter(&year, &month, &day, &hour, &minute, &second, 5);
		break;
	case 'F':
		timealter(&year, &month, &day, &hour, &minute, &second, -5);
		break;
	case 's':
		timealter(&year, &month, &day, &hour, &minute, &second, 6);
		break;
	case 'S':
		timealter(&year, &month, &day, &hour, &minute, &second, -6);
		break;
	case 'c':
		s = (s > 2)? 0 : s + 1;
		break;
	default:
		break;
	}
	inputcele(&sun, 11);
	inputcele(&moon, 10);
	if (s == 1)
	{
		persp[0] = 30.0; persp[1] = 1.0; persp[2] = 20.0;
		lookat[0] = kes*sun.x/res; lookat[1] = kes*sun.y/res; lookat[2] = kes*sun.z/res + 10.0;
		lookat[3] = kes*sun.x/res; lookat[4] = kes*sun.y/res; lookat[5] = kes*sun.z/res;
		lookat[6] = 0.0; lookat[7] = 1.0; lookat[8] = 0.0;
	}
	else if (s == 2)
	{
		persp[0] = 30.0; persp[1] = 1.0; persp[2] = 20.0;
		lookat[0] = kes*sun.x/res + 10.0; lookat[1] = kes*sun.y/res; lookat[2] = kes*sun.z/res;
		lookat[3] = kes*sun.x/res; lookat[4] = kes*sun.y/res; lookat[5] = kes*sun.z/res;
		lookat[6] = 0.0; lookat[7] = 0.0; lookat[8] = 1.0;
	}
	else if (s > 2)
	{
		persp[0] = 30.0; persp[1] = 1.0; persp[2] = 20.0;
		lookat[0] = kem*moon.x/res*3; lookat[1] = kem*moon.y/res*3; lookat[2] = kem*moon.z/res*3;
		lookat[3] = 0.0; lookat[4] = 0.0; lookat[5] = 0.0;
		lookat[6] = 0.0; lookat[7] = 0.0; lookat[8] = 1.0;
	}
	else
	{
		persp[0] = 30.0; persp[1] = 1.0; persp[2] = 20.0;
		lookat[0] = 0.0; lookat[1] = 0.0; lookat[2] = 10.0;
		lookat[3] = 0.0; lookat[4] = 0.0; lookat[5] = 0.0;
		lookat[6] = 0.0; lookat[7] = 1.0; lookat[8] = 0.0;
	}
	glutPostRedisplay();
}

//日期比较函数
int comp(int a10, int a20, int a30, int a40, int a50, float a60, int a11, int a21, int a31, int a41, int a51, float a61)
{
	if (a10 != a11)
		return (a10 < a11);
	else if (a20 != a21)
		return (a20 < a21);
	else if (a30 != a31)
		return (a30 < a31);
	else if (a40 != a41)
		return (a40 < a41);
	else if (a50 != a51)
		return (a50 < a51);
	else
		return (a60 < a61);
}

//JPL Horizons 对比数据输出
void outputfile1()
{
	FILE *fp;
	const double au = 149597870.700; 
	const double pi = 3.1415926536;
	double deldot, sot, sto;
	double des, dem, dsm;
	int years ,months, days, hours, minutes;
	float seconds;

	fp = fopen("output_horizons.txt", "w");
	fprintf(fp, "          date                delta         deldot      sot      sto\n");
	years = year;
	months = month;
	days = day;
	hours = hour;
	minutes = minute;
	seconds = second;
	while (comp(year ,month, day, hour, minute, second, yeart ,montht, dayt, hourt, minutet, secondt))
	{
		inputcele(&sun, 11);
		inputcele(&moon, 10);
		des = sqrt(pow(sun.x, 2) + pow(sun.y, 2) + pow(sun.z, 2));
		dem = sqrt(pow(moon.x, 2) + pow(moon.y, 2) + pow(moon.z, 2));
		dsm = sqrt(pow(sun.x - moon.x, 2) + pow(sun.y - moon.y, 2) + pow(sun.z - moon.z, 2));
		sot = 180 / pi * acos((pow(des, 2) + pow(dem, 2) - pow(dsm, 2)) / (2 * des * dem));
		sto = 180 / pi * acos((pow(dsm, 2) + pow(dem, 2) - pow(des, 2)) / (2 * dsm * dem));
		deldot = ((moon.u * moon.x) + (moon.v * moon.y) + (moon.w * moon.z)) / dem;
		fprintf(fp, "%4d-%2d-%2d, %2d:%2d:%5.2f %16.13lf %11.7lf %8.4lf %8.4lf\n", year, month ,day ,hour, minute, second, dem / au, deldot, sot, sto);
		timealter(&year, &month, &day, &hour, &minute, &second, step);
	}
	year = years;
	month = months;
	day = days;
	hour = hours;
	minute = minutes;
	second = seconds;
	fclose(fp);
}

//月球赤纬输出
void outputfile2()
{
	FILE *fp;
	const double pi = 3.1415926536;
	double latitude;
	int years ,months, days, hours, minutes;
	float seconds;

	fp = fopen("output_latitude.txt", "w");
	fprintf(fp, "          date           latitude\n");
	years = year;
	months = month;
	days = day;
	hours = hour;
	minutes = minute;
	seconds = second;
	while (comp(year ,month, day, hour, minute, second, yeart ,montht, dayt, hourt, minutet, secondt))
	{
		inputcele(&moon, 10);
		latitude = 180 / pi * atan(moon.z / sqrt(pow(moon.x, 2) + pow(moon.y, 2)));
		fprintf(fp, "%4d-%2d-%2d, %2d:%2d:%5.2f %8.4lf\n", year, month ,day , hour, minute, second, latitude);
		timealter(&year, &month, &day, &hour, &minute, &second, step);
	}
	year = years;
	month = months;
	day = days;
	hour = hours;
	minute = minutes;
	second = seconds;
	fclose(fp);
}

//月表太阳辐射强度输出
void outputfile3()
{
	FILE *fp;
	const double au = 149597870.700; 
	const double solartemp = 5778; //太阳表面（光球）温度，K
	const double sigma = 5.67e-8;
	double dsm, e;
	int years ,months, days, hours, minutes;
	float seconds;

	fp = fopen("output_solarrad.txt", "w");
	fprintf(fp, "          date                 dsm            e\n");
	years = year;
	months = month;
	days = day;
	hours = hour;
	minutes = minute;
	seconds = second;
	while (comp(year ,month, day, hour, minute, second, yeart ,montht, dayt, hourt, minutet, secondt))
	{
		inputcele(&sun, 11);
		inputcele(&moon, 10);
		dsm = sqrt(pow(sun.x - moon.x, 2) + pow(sun.y - moon.y, 2) + pow(sun.z - moon.z, 2));
		e = sigma*pow(solartemp, 4)*pow(rsun/dsm, 2);
		fprintf(fp, "%4d-%2d-%2d, %2d:%2d:%5.2f %16.13lf %11.7lf\n", year, month, day, hour, minute, second, dsm / au, e);
		timealter(&year, &month, &day, &hour, &minute, &second, step);
	}
	year = years;
	month = months;
	day = days;
	hour = hours;
	minute = minutes;
	second = seconds;
	fclose(fp);
}

/*————————————————————主函数————————————————————*/
int main(int argc, char **argv)
{
	printf("请输入所需星历起始时间（年、月、日、时、分、秒，格式为：XXXX XX XX XX XX XX）：\n");
	scanf("%d%d%d%d%d%f", &year , &month, &day, &hour, &minute, &second);
	printf("请输入所需星历终止时间（年、月、日、时、分、秒，格式为：XXXX XX XX XX XX XX）：\n");
	scanf("\n%d%d%d%d%d%f", &yeart , &montht, &dayt, &hourt, &minutet, &secondt);
	if (!comp(year ,month, day, hour, minute, second, yeart ,montht, dayt, hourt, minutet, secondt))
	{
		printf("错误！终止时间早于起始时间！");
		return 1;
	}
	printf("请输入相邻两数据之间的时间间隔代号（年：1 / 月：2 / 日：3 / 时：4 / 分：5 / 秒：6）：\n");
	scanf("\n%d", &step);
	if ((step < 1)||(step > 6))
	{
		printf("错误！时间间隔代号只有1、2、3、4、5、6！");
		return 1;
	}

	outputfile1();
	outputfile2();
	outputfile3();

	printf("请选择演示窗口，\n输入y/m/d/h/f/s键分别控制年/月/日/时/分/秒，\n字母大小写控制进退，\nc键控制视角\n");
	inputcele(&sun, 11);
	inputcele(&moon, 10);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE |GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(400, 50);
	glutCreateWindow(argv[0]);
	init();
	display();
	glutDisplayFunc(display);
	glutReshapeFunc(resharp);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
