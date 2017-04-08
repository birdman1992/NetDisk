; *** Inno Setup version 5.5.0+ Chinese messages ***
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/files/istrans/
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).

[LangOptions]
; The following three entries are very important. Be sure to read and?
; understand the '[LangOptions] section' topic in the help file.
LanguageName=Chinese
LanguageID=$0409
LanguageCodePage=0
; If the language you are translating to requires special font faces or
; sizes, uncomment any of the following entries and change them accordingly.
;DialogFontName=
;DialogFontSize=8
;WelcomeFontName=Verdana
;WelcomeFontSize=12
;TitleFontName=Arial
;TitleFontSize=29
;CopyrightFontName=Arial
;CopyrightFontSize=8

[Messages]

; *** Application titles
SetupAppTitle=��װ
SetupWindowTitle=��װ - %1
UninstallAppTitle=ж��
UninstallAppFullTitle=%1 ж��

; *** Misc. common
InformationTitle=��װ��Ϣ
ConfirmTitle=��ʾ
ErrorTitle=����

; *** SetupLdr messages
SetupLdrStartupMessage=���ڽ���װ %1���Ƿ����?
LdrCannotCreateTemp=���ܴ�����ʱ�ļ�����װ��ֹ��
LdrCannotExecTemp=��������ʱĿ¼�н�ѹ�ļ�����װ������ֹ��

; *** Startup error messages
LastErrorMessage=%1.%n%n ���� %2: %3
SetupFileMissing=�ڰ�װĿ¼���Ҳ����ļ� %1 ���������������߻��һ���µ��ļ���
SetupFileCorrupt=��װ�ļ���ռ�á�����һ���µİ�װ�ļ���
SetupFileCorruptOrWrongVer=��װ�ļ���ռ��, ���߰�װ�ļ��İ汾���ԡ��������������߻��һ�����ļ���
NotOnThisPlatform=�ó������� %1 �����С�
OnlyOnThisPlatform=�ó�������� %1 �����С�
OnlyOnTheseArchitectures=�ó���ֻ��������WINDOWS�汾������:%n%n%1
MissingWOW64APIs=�ð汾��Ҫ64λ�İ�װ����Ҫ���������⣬�밲װ Service Pack %1��
WinVersionTooLowError=�������Ҫ %1 �汾�� %2 ���߸��ߡ�
WinVersionTooHighError=����������� %1 �汾�� %2 ���߸��߰汾�а�װ��
AdminPrivilegesRequired=���ڰ�װ�ó���ʱ�����ǹ���ԱȨ�ޡ�
PowerUserPrivilegesRequired=���ڰ�װ�����Ǳ����ǹ���ԱȨ�޻��߸�Ȩ���û�Ȩ�ޡ�
SetupAppRunningError=��װ�����⵽ %1 �������С�%n%n��ر���������ʵ����Ȼ�󵥻���ȷ�ϡ�������װ�����ߵ������������˳���UninstallAppRunningError=ж�س����⵽ %1 �������С�%n%n��ر���������ʵ����Ȼ�󵥻���ȷ�ϡ�������װ�����ߵ������������˳���

; *** Misc. errors
ErrorCreatingDir=��װ�����ܴ���Ŀ¼ "%1"
ErrorTooManyFilesInDir=������Ŀ¼"%1" �´����ļ�����Ϊ��Ŀ¼����̫����ļ��ˡ�

; *** Setup common messages
ExitSetupTitle=�˳���װ
ExitSetupMessage=��װû����ɡ� ����������˳������򽫲�����װ��%n%n�������´�����ɰ�װ����%n%n�˳���װ?
AboutSetupMenuItem=���ڰ�װ����[&A]...
AboutSetupTitle=���ڰ�װ����
AboutSetupMessage=%1 �汾 %2%n%3%n%n%1 ��ҳ:%n%4
AboutSetupNote=
TranslatorNote=

; *** Buttons
ButtonBack=< ����[&B]
ButtonNext=��һ��[&N] >
ButtonInstall=��װ[&I]
ButtonOK=ȷ��
ButtonCancel=����
ButtonYes=��[&Y]
ButtonYesToAll=ȫ��ȷ��[&A]
ButtonNo=��[&N]
ButtonNoToAll=ȫ������[&o]
ButtonFinish=���[&F]
ButtonBrowse=���[&B]...
ButtonWizardBrowse=���[&r]...
ButtonNewFolder=�½��ļ���[&M]

; *** "Select Language" dialog messages
SelectLanguageTitle=ѡ��װ��������
SelectLanguageLabel=ѡ���������Ϊ��װ����:

; *** Common wizard text
ClickNext=��������һ�������������ߵ������������˳�����װ����
BeveledLabel=
BrowseDialogTitle=����ļ���
BrowseDialogLabel=��������б���ѡ��һ��Ŀ¼, Ȼ�󵥻���ȷ�ϡ�������
NewFolderName=���ļ���

; *** "Welcome" wizard page
WelcomeLabel1=��ӭ���� [name] ��װ��
WelcomeLabel2=�������ĵ����ϰ�װ [name/ver] ��%n%n�����ڼ�����װ֮ǰ�˳�������������С�

; *** "Password" wizard page
WizardPassword=����
PasswordLabel1=����װ���������뱣����
PasswordLabel3=���������룬�������ִ�Сд��Ȼ�󵥻�����һ����������
PasswordEditLabel=����[&P]:
IncorrectPassword=��������벻��ȷ�����������롣

; *** "License Agreement" wizard page
WizardLicense=�û����Э��
LicenseLabel=���ڼ���֮ǰ��ϸ�Ķ��û����Э�顣
LicenseLabel3=����ϸ�Ķ�������û����Э�顣�ڼ�����װ֮ǰ����������ܸ�Э�顣
LicenseAccepted=�ҽ���Э��[&a]��
LicenseNotAccepted=�Ҳ�����Э��[&d]

; *** "Information" wizard pages
WizardInfoBefore=��ʾ
InfoBeforeLabel=�ڼ�����װ֮ǰ����ϸ�Ķ�������Ҫ����Ϣ��
InfoBeforeClickLabel=����׼���ü�����װʱ����������������
WizardInfoAfter=��Ϣ
InfoAfterLabel=�ڼ�����װ֮ǰ����ϸ�Ķ�������Ҫ����Ϣ��
InfoAfterClickLabel=����׼���ü�����װʱ����������������

; *** "User Information" wizard page
WizardUserInfo=�û���Ϣ
UserInfoDesc=������������Ϣ��
UserInfoName=�û���[&U]:
UserInfoOrg=��֯[&O]:
UserInfoSerial=���к�[&S]:
UserInfoNameRequired=����������һ�����ơ�

; *** "Select Destination Location" wizard page
WizardSelectDir=ѡ��Ŀ��λ��
SelectDirDesc=������[name]��װ������?
SelectDirLabel3=��װ���򽫰�[name]��װ������ĵ��ļ����С�
SelectDirBrowseLabel=Ϊ�˼�����װ, �뵥������һ�������������ѡ��һ����ͬ��Ŀ¼���뵥�����������
DiskSpaceMBLabel=Ϊ�˰�װ�������������Ҫ [mb] MB �Ŀ��д��̿ռ䡣
ToUNCPathname=��װ�����ܰ�װ��һ�� UNC Ŀ¼���ơ��������ͼ���ϰ�װ��������ӳ��������������
InvalidPath=����������һ�����̷���·�����ơ�����:%n%nC:\APP%n%n ����һ������·��������:%n%n\\server\share
InvalidDrive=��ѡ��Ĵ��̻�������·�������ڻ��߲��ܷ��ʡ�������ѡ�������Ĵ��̻�������·����
DiskSpaceWarningTitle=���̿ռ䲻�㡣
DiskSpaceWarning=��װ����������Ҫ %1 KB ���д��̿ռ�����װ�����������ѡ��Ĵ�����ֻ�� %2 KB �ռ���á�%n%n ��ȷ�ϼ�����
DirNameTooLong=���ļ��е�����̫����
InvalidDirName=���ļ��е�����̫����
BadDirName32=�ļ��������в��ܰ������µ��κ��ַ�:%n%n%1
DirExistsTitle=�ļ����Ѿ�����
DirExists=�ļ���:%n%n%1%n%n�Ѿ����ڡ����������װ?
DirDoesntExistTitle=�ļ��в�����
DirDoesntExist=�ļ���:%n%n%1%n%n�����ڡ����봴�����ļ���?

; *** "Select Components" wizard page
WizardSelectComponents=ѡ�񲿼�
SelectComponentsDesc=��Щ������Ҫ��װ?
SelectComponentsLabel2=ѡ������Ҫ��װ�Ĳ���; ��������밲װ�Ĳ���������׼���ú��뵥������һ���� ��
FullInstallation=��ȫ��װ
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=����װ
CustomInstallation=�Զ��尲װ
NoUninstallWarningTitle=��������
NoUninstallWarning=��װ�����⵽���²����Ѿ������ĵ����б���װ��:%n%n%1%n%n��ѡ�����ǽ��������ĵ����а�װ���ǡ�%n%n���������������?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=��ǰѡ����������Ҫ [mb] MB �Ŀ��д��̿ռ���ܰ�װ��

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=ѡ��һ����������
SelectTasksDesc=����ѡ���ĸ���������?
SelectTasksLabel2=ѡ���ڰ�װ[name]ʱִ�еĸ�������, Ȼ�󵥻�����һ������

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=ѡ��ʼ�˵��ļ���
SelectStartMenuFolderDesc=�����������������Ŀ�ݷ�ʽ?
SelectStartMenuFolderLabel3=��װ������������ļ����д�������Ŀ�ݷ�ʽ��
SelectStartMenuFolderBrowseLabel=Ϊ�˼�������������һ�����������������ѡ��һ���ļ��У��������������
MustEnterGroupName=����������һ���ļ������ơ�
GroupNameTooLong=�ļ������ƻ���·������̫����
InvalidGroupName=�ļ������ƷǷ���
BadGroupName=�ļ��������в��ܰ���������ַ�:%n%n%1
NoProgramGroupCheck2=��������ʼ�˵��е��ļ���[&D]

; *** "Ready to Install" wizard page
WizardReady=׼����ʼ��װ
ReadyLabel1=��װ����ʼ�����ĵ����а�װ[name].
ReadyLabel2a=��������װ����ʼ��װ�����,���ߵ��������ˡ��޸İ�װ���á�
ReadyLabel2b=��������װ����ʼ��װ�������
ReadyMemoUserInfo=�û���Ϣ:
ReadyMemoDir=��װĿ��λ��:
ReadyMemoType=��װ����:
ReadyMemoComponents=ѡ�еĲ���:
ReadyMemoGroup=��ʼ�˵��ļ���:
ReadyMemoTasks=��������:

; *** "Preparing to Install" wizard page
WizardPreparing=׼����װ
PreparingDesc=��װ����׼�������ĵ����а�װ[name]��
PreviousInstallNotCompleted=��װ/ж��һ����ǰ�ĳ���û����ɡ�����Ҫ�����������ĵ�������ɰ�װ������%n%n�������������ĵ��Ժ������а�װ��������ɰ�װ[name]��
CannotContinue=��װ�����ܼ���ִ�С��뵥�����������˳���

; *** "Installing" wizard page
WizardInstalling=��װ��
InstallingLabel=��װ�������ڰ�װ[name],��ȴ���

; *** "Setup Completed" wizard page
FinishedHeadingLabel=[name]��װ���
FinishedLabelNoIcons=��װ�����Ѿ������ĵ����а�װ��[name]��
FinishedLabel=��װ�����Ѿ������ĵ����а�װ��[name]��Ҫִ�б�������뵥����װ�õı����ͼ�� ��
ClickFinish=��������ɡ��˳�����װ����.
FinishedRestartLabel=Ϊ�����[name]�İ�װ, ��װ������������������ĵ��ԡ��������ھ���������?
FinishedRestartMessage=Ϊ�����[name]�İ�װ, ��װ������������������ĵ��ԡ�%n%�������ھ���������?
ShowReadmeCheck=�ǵ�,����鿴 README �ļ�
YesRadio=�ǵģ������������������[&Y]
NoRadio=�����ҽ��Ժ��������������[&N]
; used for example as 'Run MyProg.exe'
RunEntryExec=���� %1
; used for example as 'View Readme.txt'
RunEntryShellExec=�鿴 %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=��װ������Ҫ��һ����װ��
SelectDiskLabel2=����밲װ�� %1 ���ҵ�����ȷ�ϡ���%n%n������̲���������ʾ���̣���������ȷ��·�����ߵ������������
PathLabel=Ŀ¼[&P]:
FileNotInDir2=�� "%2" ��û�з����ļ����������ȷ�Ĵ��̻���ѡ���������ļ��С�
SelectDirectoryLabel=��������һ�������е���ȷλ�á�

; *** Installation phase messages
SetupAborted=��װû����ɡ�%n%n���޸��������°�װ��
EntryAbortRetryIgnore=���������ԡ����³��ԣ����������ԡ���������װ�����ߵ������������˳���װ��

; *** Installation status messages
StatusCreateDirs=����Ŀ¼��...
StatusExtractFiles=��ѹ�ļ���...
StatusCreateIcons=������ݷ�ʽ��...
StatusCreateIniEntries=����INI��Ԫ��...
StatusCreateRegistryEntries=����ע���������...
StatusRegisterFiles=ע���ļ���...
StatusSavingUninstall=����ж����Ϣ��...
StatusRunProgram=������ɰ�װ...
StatusRollback=�ָ�ԭ���޸ĵ�������...

; *** Misc. errors
ErrorInternal2=�ڲ�����: %1
ErrorFunctionFailedNoCode=%1 ʧ��
ErrorFunctionFailed=%1 ʧ��; ���� %2
ErrorFunctionFailedWithMessage=%1 ����; ���� %2.%n%3
ErrorExecutingProgram=����ִ���ļ�:%n%1

; *** Registry errors
ErrorRegOpenKey=�ڴ�ע����ʱ��������:%n%1\%2
ErrorRegCreateKey=�ڴ���ע����ʱ��������:%n%1\%2
ErrorRegWriteKey=��Щע����ʱ��������:%n%1\%2

; *** INI errors
ErrorIniEntry=�ڴ���INI�ļ�ʱ�������� "%1".

; *** File copying errors
FileAbortRetryIgnore=���������ԡ�����һ�Σ����������ԡ����Ը��ļ������ߵ������������˳���װ����
FileAbortRetryIgnore2=���������ԡ�����һ�Σ����������ԡ����Ը��ļ�������װ�����ߵ������������˳���װ����
SourceIsCorrupted=Դ�ļ���ʹ��
SourceDoesntExist=Դ�ļ� "%1" ������
ExistingFileReadOnly=���Ѿ����ڵ��ļ���ֻ�����ԡ�%n%n���������ԡ�ɾ��ֻ�����Բ����³��ԣ����������ԡ����Ը��ļ������ߵ������������˳���װ����
ErrorReadingExistingDest=����ͼ��ȡһ���Ѿ����ڵ��ļ�ʱ�����˴���:
FileExists=���ļ��Ѿ����ڡ�%n%n���븲������?
ExistingFileNewer=�Ѿ����ڵ��ļ��Ȱ�װ������ͼ��װ���ļ�Ҫ�¡��������������ļ���%n%n���뱣���Ѿ����ڵ��ļ���?
ErrorChangingAttr=����ͼ�ı�һ�����ڵ��ļ�������ʱ�����˴���:
ErrorCreatingTemp=����ͼ��Ŀ��Ŀ¼�д���һ���ļ�ʱ�����˴���:
ErrorReadingSource=����ͼ��ȡһ���ļ�ʱ�����˴���:
ErrorCopying=����ͼ����һ���ļ�ʱ�����˴���:
ErrorReplacingExistingFile=����ͼ�����Ѿ����ڵ��ļ�ʱ��������:
ErrorRestartReplace=���������û�ʧ��:
ErrorRenamingTemp=����Ŀ��Ŀ¼���������ļ�ʱ��������:
ErrorRegisterServer=����ע�� DLL/OCX: %1
ErrorRegisterServerMissingExport=û��Dllע���������
ErrorRegisterTypeLib=����ע���������Ϳ�: %1

; *** Post-installation errors
ErrorOpeningReadme=���� README �ļ�ʱ��������
ErrorRestartingComputer=��װ�����������������ԡ����ֶ�������

; *** Uninstaller messages
UninstallNotFound=�ļ� "%1" �����ڡ�����ж�ء�
UninstallOpenError=�ļ� "%1" ���ܴ򿪡�����ж�ء�
UninstallUnsupportedVer=ж�ؼ�¼�ļ� "%1" ���ǻ��ڱ���װ����İ汾��������������ɾ��������
UninstallUnknownEntry=һ��δ֪��ʵ�� (%1) ������ж�ؼ�¼�ļ��С�
ConfirmUninstall=��ȷ����ȫɾ�� %1 �����л���������Ĳ�����?
UninstallOnlyOnWin64=�ð�װ����ֻ����64λ��Windows��ִ��ж�ع�����
OnlyAdminCanUninstall=�ð�װ����ֻ���������й���ԱȨ��ʱ����ִ��ж�ع�����
UninstallStatusLabel= %1 ���ڱ�ɾ�������Եȡ�
UninstalledAll=%1 ���ɹ��ش����ĵ�����ɾ����
UninstalledMost=%1 ɾ����ɡ�%n%n��ĳЩ�������ܱ�ɾ������������Ҫ�ֶ�ɾ�����ǡ�
UninstalledAndNeedsRestart=Ϊ����� %1 ��ɾ������, �����������������ԡ�%n%n�������ھ�����������?
UninstallDataCorrupted=�ļ�"%1" ��ռ�á��������ɾ��������

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=ɾ����������ļ�?
ConfirmDeleteSharedFile2=ϵͳָ�����µĹ�������ļ������ٱ�ʹ�á����Ƿ���ɾ����Щ�����ļ�?%n%n�����Щ�ļ�ɾ��������������ȻҪʹ������������������Ĺ��ܽ���Ӱ�졣��������ܿ϶�����ѡ�񡰷񡱡�����Щ�ļ�������ϵͳ�в����ϵͳ����𺦡�
SharedFileNameLabel=�ļ���:
SharedFileLocationLabel=λ��:
WizardUninstalling=ж��
StatusUninstalling=ж�� %1 ��...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]

NameAndVersion=%1 �汾 %2
AdditionalIcons=����ͼ��:
CreateDesktopIcon=��������ͼ��[&d]
CreateQuickLaunchIcon=������������ͼ��[&Q]
ProgramOnTheWeb=%1 on the Web
UninstallProgram=ж�� %1
LaunchProgram=ִ�� %1
AssocFileExtension=���ļ���չ�� %2 ƥ��[&A] %1?
AssocingFileExtension=�������ļ���չ�� %2 ƥ��[&A] %1 ��...