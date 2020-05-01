#pragma once
#include "../WorkerThreads/LogWatcher/LogWatcher.h"
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "../WorkerThreads/LogWatcher/LogMessageEvent.h"
#include <wx/grid.h>

class LogWindow: public wxWindow
{
  public:
	LogWindow(wxWindow* parent);

	void initializeTail();
	void initializeSecondTail(const std::string& tailSource);
	void terminateSecondTail() const;
	void OnTailPush(LogMessageEvent& event);

  private:
	LogWatcher* logWatcher = nullptr;
	LogWatcher* logWatcher2 = nullptr;
	int logCounter = 0;
	wxGrid* theGrid = nullptr;
	std::vector<wxStaticText*> logArray;
};