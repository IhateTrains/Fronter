#include "LogWindow.h"
#include "../LogWatcher/LogWatcher.h"
#include "Log.h"

BEGIN_EVENT_TABLE(LogWindow, wxScrolledWindow)
EVT_COMMAND(wxID_ANY, wxEVT_TAILTHREAD, LogWindow::OnTailPush)
END_EVENT_TABLE()

LogWindow::LogWindow(wxWindow* parent): wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, -1))
{
	SetScrollRate(0, 20);
	wxBoxSizer* logBox = new wxBoxSizer(wxVERTICAL);
	SetSizer(logBox);
	initializeTail();
}

void LogWindow::initializeTail()
{
	LogWatcher* logWatcher = new LogWatcher(this);
	logWatcher->Create();
	logWatcher->Run();
}
void LogWindow::OnTailPush(wxCommandEvent& event)
{
	logCounter++;
	const auto message = event.GetString();
	wxStaticText* st = new wxStaticText(this, wxID_ANY, message, wxDefaultPosition, wxSize(-1, 18));
	GetSizer()->Add(st, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER | wxEXPAND | wxALL, 1);
	GetParent()->Layout();
	Scroll(0, logCounter);
}