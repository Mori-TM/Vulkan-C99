#define OBJ  "OBJ Files\0*.obj*\0"
#define FBX  "FBX Files\0*.fbx*\0"
#define GLB  "GLB Files\0*.glb*\0"
#define GLTF "GLTF Files\0*.gltf*\0"
#define DAE  "DAE Files\0*.dae*\0"
#define OBJDAESTLWAVE "All Models\0*.obj;*.dae;*.stl;*.wave\0"

#define PNG  "PNG Files\0*.png*\0"
#define JPG  "JPG Files\0*.jpg*\0"
#define BMP  "BMP Files\0*.bmp*\0"
#define JPEG "JPEG Files\0*.jpeg*\0"
#define TGA  "TGA Files\0*.tga*\0"
#define GIF  "GIF Files\0*.gif*\0"
#define PSD  "PSD Files\0*.psd*\0"
#define HDR  "HDR Files\0*.hdr*\0"

#define ALLIMAGES "All Images\0*.png;*.jpg;*.bmp;*.jpeg;*.tga;*.gif;*.psd;*.hdr\0"

#define MP3 "MP3 Files\0*.mp3*\0"
#define OGG "OGG Files\0*.ogg*\0"
#define WAV "WAV Files\0*.wav*\0"
#define ALLAUDIOS "All Audios\0*.mp3;*.ogg;*.wav\0"

#define TXT "TXT Files\0*.txt*\0"

#define ALL "All Files\0*.*\0"

#define MAG "Mag Files\0*.mag*\0"
#define LAVA "Lava Files\0*.lav*\0"
#define LUA "Lua Files\0*.lua*\0"

#ifdef _WIN32
#include <Windows.h>
HWND Hwnd;

BOOL OpenDialog(char* DstPath, const char *Filter)
{
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = Hwnd;
	ofn.lpstrFile = DstPath;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 1000;
	ofn.lpstrFilter = Filter;
	ofn.nFilterIndex = 1;

	return GetOpenFileName(&ofn);
}

BOOL SaveDialog(char* DstPath, const char* Filter)
{
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = Hwnd;
	ofn.lpstrFile = DstPath;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 1000;
	ofn.lpstrFilter = Filter;
	ofn.nFilterIndex = 1;

	return GetSaveFileName(&ofn);
}

#elif __linux__
#include <gtk/gtk.h>
#include <gdk/gdkx.h>

bool OpenDialog(char* DstPath, const char* Filter)
{
	GtkWidget* Dialog;
	gint Res;

	Dialog = gtk_file_chooser_dialog_new("Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);
	Res = gtk_dialog_run(GTK_DIALOG(Dialog));

	if (Res == GTK_RESPONSE_ACCEPT)
	{
		GtkFileChooser* chooser = GTK_FILE_CHOOSER(Dialog);
		DstPath = gtk_file_chooser_get_filename(chooser);
		open_file(DstPath);
		g_free(DstPath);

		return true;
	}
	else
		return false;

	gtk_widget_destroy(Dialog);
}

bool SaveDialog(char* DstPath, const char* Filter)
{
	GtkWidget* Dialog;
	gint Res;

	Dialog = gtk_file_chooser_dialog_new("Save File", NULL, GTK_FILE_CHOOSER_ACTION_SAVE, "_Cancel", GTK_RESPONSE_CANCEL, NULL);
	Res = gtk_dialog_run(GTK_DIALOG(Dialog));

	if (Res == GTK_RESPONSE_ACCEPT)
	{
		GtkFileChooser* chooser = GTK_FILE_CHOOSER(Dialog);
		DstPath = gtk_file_chooser_get_filename(chooser);
		open_file(DstPath);
		g_free(DstPath);

		return true;
	}
	else
		return false;

	gtk_widget_destroy(Dialog);
}

#endif