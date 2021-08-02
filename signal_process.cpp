
#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include <thread>
#include <complex> 
#include "fftw3.h"  
#include <windows.h>
#include<graphics.h>
#include<conio.h>
#include<math.h>


#pragma comment(lib, "ws2_32.lib")
using namespace std;


void caiji (char** ans, int* flag)
{
    string cunchu = "";
    int  b,a;
    char alldata1[131072];
    char alldata2[131072];
    memset(alldata1, 0, sizeof(alldata1));
    int i = 0;
    WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 2);
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
       
    }

    SOCKET serSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serSocket == INVALID_SOCKET)
    {

      
    }

    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(4098);
    serAddr.sin_addr.S_un.S_addr = inet_addr("192.168.33.30");

    if (bind(serSocket, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {

        closesocket(serSocket);
       
    }

    sockaddr_in remoteAddr;
    int nAddrLen = sizeof(remoteAddr);
    while (true)
    {
        char recvData[1466];
        int ret = recvfrom(serSocket, recvData, 1466, 0, (sockaddr*)&remoteAddr, &nAddrLen);
        //printf("%d\n", ret);
        if (ret > 0)
        {

            //printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));
            //printf(recvData);
           // printf("%x\n", unsigned char(recvData[1464]));


            b = (((unsigned char)recvData[3]) << 24) + (((unsigned char)recvData[2]) << 16) + (((unsigned char)recvData[1]) << 8) + (unsigned char)recvData[0];
            printf("%d\n", b);

            a = (((unsigned char)recvData[9]) << 40) + (((unsigned char)recvData[8]) << 32) + (((unsigned char)recvData[7]) << 24)
                + (((unsigned char)recvData[6]) << 16) + (((unsigned char)recvData[5]) << 8) + (unsigned char)recvData[4];

            memcpy(alldata1 + (a%131072), recvData + 10, (ret - 10));
            
            if (b % 91 == 0 && *flag==0)
            {
                memcpy(alldata2, alldata1, sizeof(alldata1));
                memset(alldata1, 0, sizeof(alldata1));
                *ans = alldata2;
                
                *flag = 1;
            }
            
         
        }

    }
    closesocket(serSocket);
    WSACleanup();
}

int main(int argc, char* argv[])
{
    double c = 3.0e8;
    double B = 3942.7e6;
    double K = 68e12;
    double T = B / K;
    double Tc = 59;
    double fs = 5.5e6;
    double f0 = 77e9;
    double lambdas = c / f0;
    double d = lambdas / 2;
    int n_samples = 256;
    double N = 256;
    int n_chirps = 32;
    double M = 32;
    double n_RX = 4;
    double Q = 180;
    double xx = 1;
    double sdata[65536];
    fftw_complex* in, * out;
    fftw_plan p;
    int N_fft = 256;
    in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    complex<double> lvds_data[1][32768];
    complex<double> lvds_data1[1024][32];
    complex<double> lvds_data2[32][1024];
    complex<double> cdata[4][8192];
    complex<double> data_radar_1[256][32];
    complex<double> data_radar_2[256][32];
    complex<double> data_radar_3[256][32];
    complex<double> data_radar_4[256][32];
    complex<double> data_radar[256][32][4];
    complex<double> range_profile[256][32][4];
    double range_fft[256];
    int range_fft_huatu[256];
    initgraph(1200, 600);
    setorigin(10, 580);    //重置坐标原点
    setbkcolor(WHITE);   //设置背景颜色
    cleardevice();
    setlinecolor(BLACK);
    line(0, 0, 1180, 0);
    line(0, 0, 0, -560);
    memset(sdata, 0, sizeof(sdata));
    int* sum1 = new int(0);



    char* allData = new char[131072];
    thread threadObj(caiji, &allData,sum1);
   // thread threadObj(caiji, sum1);
    threadObj.detach();


    int k = 0 ,m = 0;
    while (true)
    {

        k =(unsigned char)allData[131071];
        m = (unsigned char)allData[131070];
        
        if (*sum1 == 1)
        {

            int  i1 = 0;
            while (i1 < 65536)
            {
                sdata[i1] = ((allData[i1 * 2 + 1]) << 8) + allData[i1 * 2];
                i1++;
            }

            int count = 0;

            for (int i2 = 0; i2 < 65532; i2 = i2 + 4)
            {
                lvds_data[0][count] = complex<double>(sdata[i2], sdata[i2 + 2]);
                lvds_data[0][count + 1] = complex<double>(sdata[i2 + 1], sdata[i2 + 3]);
                count = count + 2;
            }

            for (int k1 = 0; k1 < 32; k1++)
            {
                for (int k2 = 0; k2 < 1024; k2++)
                {
                    lvds_data1[k2][k1] = lvds_data[0][k1 * 1024 + k2];
                }
            }

            for (int k3 = 0; k3 < 32; k3++)
            {
                for (int k4 = 0; k4 < 1024; k4++)
                {
                    lvds_data2[k3][k4] = lvds_data1[k4][k3];
                }
            }

            for (int k5 = 0; k5 < n_chirps; k5++)
            {
                for (int k6 = 0; k6 < n_samples; k6++)
                {
                    cdata[0][k5 * n_samples + k6] = lvds_data2[k5][k6];
                }
            }
            for (int k5 = 0; k5 < n_chirps; k5++)
            {
                for (int k6 = 0; k6 < n_samples; k6++)
                {
                    cdata[1][k5 * n_samples + k6] = lvds_data2[k5][n_samples + k6];
                }
            }
            for (int k5 = 0; k5 < n_chirps; k5++)
            {
                for (int k6 = 0; k6 < n_samples; k6++)
                {
                    cdata[2][k5 * n_samples + k6] = lvds_data2[k5][2 * n_samples + k6];
                }
            }
            for (int k5 = 0; k5 < n_chirps; k5++)
            {
                for (int k6 = 0; k6 < n_samples; k6++)
                {
                    cdata[3][k5 * n_samples + k6] = lvds_data2[k5][3 * n_samples + k6];
                }
            }
            //

            for (int k7 = 0; k7 < n_samples; k7++)
            {
                for (int k8 = 0; k8 < n_chirps; k8++)
                {
                    data_radar_1[k7][k8] = cdata[0][k7 + k8 * n_samples];
                }
            }

            for (int k8 = 0; k8 < n_samples; k8++)
            {
                for (int k9 = 0; k9 < n_chirps; k9++)
                {
                    data_radar_2[k8][k9] = cdata[1][k8 + k9 * n_samples];
                }
            }

            for (int k10 = 0; k10 < n_samples; k10++)
            {
                for (int k11 = 0; k11 < n_chirps; k11++)
                {
                    data_radar_3[k10][k11] = cdata[2][k10 + k11 * n_samples];
                }
            }
            for (int k12 = 0; k12 < n_samples; k12++)
            {
                for (int k13 = 0; k13 < n_chirps; k13++)
                {
                    data_radar_4[k12][k13] = cdata[3][k12 + k13 * n_samples];
                }
            }

            //
            //
            for (int k14 = 0; k14 < n_samples; k14++)
            {
                for (int k15 = 0; k15 < n_chirps; k15++)
                {
                    data_radar[k14][k15][0] = data_radar_1[k14][k15];
                }
            }
            for (int k14 = 0; k14 < n_samples; k14++)
            {
                for (int k15 = 0; k15 < n_chirps; k15++)
                {
                    data_radar[k14][k15][1] = data_radar_2[k14][k15];
                }
            }
            for (int k14 = 0; k14 < n_samples; k14++)
            {
                for (int k15 = 0; k15 < n_chirps; k15++)
                {
                    data_radar[k14][k15][2] = data_radar_3[k14][k15];
                }

            }
            for (int k14 = 0; k14 < n_samples; k14++)
            {
                for (int k15 = 0; k15 < n_chirps; k15++)
                {
                    data_radar[k14][k15][3] = data_radar_4[k14][k15];
                }
            }
            // range fft
            for (int k16 = 0; k16 < n_RX; k16++)
            {
                for (int k17 = 0; k17 < n_chirps; k17++)
                {
                    for (int k18 = 0; k18 < N_fft; k18++)
                    {
                        in[k18][0] = data_radar[k18][k17][k16].real();
                        in[k18][1] = data_radar[k18][k17][k16].imag();
                    }

                    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
                    fftw_execute(p);
                    for (int k19 = 0; k19 < N_fft; k19++)
                    {
                        range_profile[k19][k17][k16] = complex<double>(out[k19][0], out[k19][1]);
                    }
                }
            }

            for (int k20 = 0; k20 < N_fft; k20++)
            {
                range_fft[k20] = abs(range_profile[k20][0][0]);
            }


            for (int k21 = 0; k21 < N_fft; k21++)
            {
                range_fft_huatu[k21] = abs(range_profile[k21][0][0]);
            }


            setlinecolor(0x24c097);
            setlinestyle(PS_SOLID | PS_ENDCAP_FLAT, 2);  //设置画笔样式
            moveto(0, -range_fft_huatu[0] / 20);

            for (int i = 1; i < 256; i++)
            {
                lineto(5 * (i + 1), -range_fft_huatu[i] / 100);
            }
            

        }
        
    }

    return 0;
}