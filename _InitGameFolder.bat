@echo off

echo --------------------------------------------------------------------------
echo �s�v�t�H���_���폜���܂��B
echo �r���h�ł��Ȃ����A�v���W�F�N�g���N���ł��Ȃ��Ƃ��Ɏg���Ă��������B
echo;  
echo �폜���I�������AGenerate Visual Studio project files���s���Ă��������B
echo --------------------------------------------------------------------------
echo;

rem       ���̖��O�̓v���W�F�N�g�������Ă�ꏊ�ɂȂ�܂�
set dir="Game"

rem �t�H���_�K�w�̈ړ�-------------------------
echo;
echo �t�H���_ %dir%/�Ɉړ����܂�
cd %dir%/
rem -------------------------------------------

rem �t�H���_�폜-------------------------------
rd /s .vs
echo;
rd /s Binaries
echo;
rd /s Build
echo;
rd /s DerivedDataCache
echo;
rd /s Intermediate
echo;
rd /s Saved
echo;
del /p Game.sln
rem -------------------------------------------