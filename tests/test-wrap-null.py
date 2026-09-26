#!/usr/bin/env python3
"""Run the actual W3D_SetWrapMode with a NULL and a supplied border color."""
import pathlib, subprocess, tempfile
root = pathlib.Path(__file__).resolve().parents[1]
source = (root / 'Warp3D.library/Texture.c').read_text()
body = source[source.index('ULONG W3D_SetWrapMode('):source.index('ULONG W3D_UpdateTexImage(')]
envbody = source[source.index('ULONG W3D_SetTexEnv('):source.index('ULONG W3D_SetWrapMode(')]
assert 'malloc(' not in envbody
mock = r'''
#include <assert.h>
#include <stdio.h>
typedef unsigned long ULONG;
typedef float GLfloat;
typedef struct {float r,g,b,a;} W3D_Color;
typedef struct {int unused;} W3D_Context;
typedef struct {void *driver;} W3D_Texture;
typedef struct {int glID; long s_mode,t_mode; W3D_Color bordercolor; ULONG envparam; W3D_Color envcolor;} Texture;
#define __REGA0(x) x
#define __REGA1(x) x
#define __REGA2(x) x
#define __REGD0(x) x
#define __REGD1(x) x
#define LOG (void)context
#define SWAP32(x,n)
#define GL_TEXTURE_2D 1
#define GL_TEXTURE_WRAP_S 2
#define GL_TEXTURE_WRAP_T 3
#define GL_TEXTURE_BORDER_COLOR 4
#define W3D_SUCCESS 0
#define W3D_BLEND 4
#define GL_TEXTURE_ENV 5
#define GL_TEXTURE_ENV_MODE 6
#define GL_TEXTURE_ENV_COLOR 7
static long envs[]={0,10,20,30,40};
static long wrap[]={0,10,20},memoffset;
static float color[4],sent[4];
static int binds,params;
static void _glBindTexture(int target,int id){assert(target==1 && id==7); ++binds;}
static void _glTexParameteri(int target,int p,long v){assert(target==1); assert((p==2 && v==10)||(p==3 && v==20)); ++params;}
static void _glTexParameterfv(int target,int p,GLfloat *v){assert(target==1 && p==4); for(int i=0;i<4;++i)sent[i]=v[i];}
static void _glTexEnvi(int target,int p,long v){assert(target==5 && p==6 && (v==30 || v==40));}
static void _glTexEnvfv(int target,int p,GLfloat *v){assert(target==5 && p==7); for(int i=0;i<4;++i)sent[i]=v[i];}
'''
test = r'''
int main(void){
 Texture t={7,0,0,{0.1f,0.2f,0.3f,0.4f},0,{0,0,0,0}}; W3D_Texture tex={&t};
 assert(W3D_SetWrapMode(NULL,&tex,1,2,NULL)==W3D_SUCCESS);
 assert(t.bordercolor.r==0.1f && t.bordercolor.g==0.2f && t.bordercolor.b==0.3f && t.bordercolor.a==0.4f);
 assert(sent[0]==0.1f && sent[1]==0.3f && sent[2]==0.2f && sent[3]==0.4f);
 W3D_Color c={0.5f,0.6f,0.7f,0.8f};
 W3D_SetWrapMode(NULL,&tex,1,2,&c);
 W3D_SetWrapMode(NULL,&tex,1,2,NULL);
 assert(t.bordercolor.r==c.r && t.bordercolor.g==c.g && t.bordercolor.b==c.b && t.bordercolor.a==c.a);
 assert(sent[0]==c.r && sent[1]==c.b && sent[2]==c.g && sent[3]==c.a);
 assert(binds==3 && params==6);
 W3D_SetTexEnv(NULL,&tex,3,NULL);
 assert(t.envparam==3 && t.envcolor.r==0 && t.envcolor.a==0);
 W3D_SetTexEnv(NULL,&tex,W3D_BLEND,&c);
 W3D_SetTexEnv(NULL,&tex,W3D_BLEND,NULL);
 assert(t.envcolor.r==c.r && t.envcolor.g==c.g && t.envcolor.b==c.b && t.envcolor.a==c.a);
 assert(sent[0]==c.r && sent[1]==c.b && sent[2]==c.g && sent[3]==c.a);
 puts("PASS: texture environment accepts NULL, preserves supplied color, no allocation");
 puts("PASS: NULL preserves border color; non-NULL updates it; wrap modes still applied");
}
'''
with tempfile.TemporaryDirectory(prefix='quarktex-wrap-') as tmp:
    for name, code in [('fixed',body+envbody),('original',body.replace('if (bordercolor) {','{')+envbody),('original-env',body+envbody.replace('if (envcolor) {','{'))]:
        exe=str(pathlib.Path(tmp)/name)
        subprocess.run(['cc','-x','c','-std=c99','-Wall','-Wextra','-Werror',
                        '-fsanitize=undefined','-fno-sanitize-recover=all','-o',exe,'-'],
                       input=mock+code+test,text=True,check=True)
        result=subprocess.run([exe],capture_output=True,text=True)
        if name=='fixed':
            assert result.returncode==0,result.stderr
            print(result.stdout.strip())
        else:
            assert result.returncode!=0 and 'null pointer' in result.stderr,result.stderr
            print('PASS: negative control detects original NULL dereference')
