#pragma once
#include "../util.h"

struct NeoniteGUI
{
	char InputBuf[256];
	ImVector<char*> Items;
	ImVector<const char*> Commands;
	ImVector<char*> History;
	int HistoryPos; // -1: new line, 0..History.Size-1 browsing history.
	ImGuiTextFilter Filter;
	bool AutoScroll;
	bool ScrollToBottom;

	NeoniteGUI()
	{
		ClearLog();
		memset(InputBuf, 0, sizeof(InputBuf));
		HistoryPos = -1;

		// "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
		Commands.push_back("HELP");
		Commands.push_back("HISTORY");
		Commands.push_back("CLEAR");
		Commands.push_back("CLASSIFY");
		AutoScroll = true;
		ScrollToBottom = false;
	}

	~NeoniteGUI()
	{
		ClearLog();
		for (int i = 0; i < History.Size; i++) free(History[i]);
	}

	// Portable helpers
	static int Stricmp(const char* s1, const char* s2)
	{
		int d;
		while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1)
		{
			s1++;
			s2++;
		}
		return d;
	}

	static int Strnicmp(const char* s1, const char* s2, int n)
	{
		int d = 0;
		while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1)
		{
			s1++;
			s2++;
			n--;
		}
		return d;
	}

	static char* Strdup(const char* s)
	{
		size_t len = strlen(s) + 1;
		void* buf = malloc(len);
		IM_ASSERT(buf);
		return static_cast<char*>(memcpy(buf, static_cast<const void*>(s), len));
	}

	static void Strtrim(char* s)
	{
		char* str_end = s + strlen(s);
		while (str_end > s && str_end[-1] == ' ') str_end--;
		*str_end = 0;
	}

	void ClearLog()
	{
		for (int i = 0; i < Items.Size; i++) free(Items[i]);
		Items.clear();
	}

	void AddLog(const char* fmt, ...) IM_FMTARGS(2)
	{
		// FIXME-OPT
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
		buf[IM_ARRAYSIZE(buf) - 1] = 0;
		va_end(args);
		Items.push_back(Strdup(buf));
	}

	void Draw(const char* title, bool* p_open)
	{
		/*ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(title, p_open))
		{
				ImGui::End();
				return;
		}

		// As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
		// So e.g. IsItemHovered() will return true when hovering the title bar.
		// Here we create a context menu only available from the title bar.
		if (ImGui::BeginPopupContextItem())
		{
				if (ImGui::MenuItem("Close Console"))
						*p_open = false;
				ImGui::EndPopup();
		}*/

		//static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

		//ImGui::Separator();

		// Options, Filter

		/*ImGui::SameLine();
		Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
		ImGui::Separator();*/

		// Reserve enough left-over height for 1 separator + 1 input text
		const float footer_height_to_reserve =
		ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve),
		                  false, ImGuiWindowFlags_HorizontalScrollbar);

		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("Clear")) ClearLog();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
		// Tighten spacing
		for (int i = 0; i < Items.Size; i++)
		{
			const char* item = Items[i];
			if (!Filter.PassFilter(item)) continue;

			// Normally you would store more information in your item than just a string.
			// (e.g. make Items[] an array of structure, store color/type etc.)
			ImVec4 color;
			bool has_color = false;
			if (strstr(item, "[+]"))
			{
				color = ImColor(0, 255, 50);
				has_color = true;
			}
			else if (strstr(item, "[x]"))
			{
				color = ImColor(255, 0, 0);
				has_color = true;
			}
			else if (strstr(item, "[=]"))
			{
				color = ImColor(85, 0, 255);
				has_color = true;
			}
			else if (strstr(item, "[>]"))
			{
				color = ImColor(135, 25, 195);
				has_color = true;
			}
			if (has_color) ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::TextUnformatted(item);
			if (has_color) ImGui::PopStyleColor();
		}

		if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >=
			ImGui::GetScrollMaxY()))
			ImGui::SetScrollHereY(1.0f);
		ScrollToBottom = false;

		ImGui::PopStyleVar();
		ImGui::EndChild();
		//        ImGui::Separator();

		// Command-line
		bool reclaim_focus = false;
		ImGuiInputTextFlags input_text_flags =
		ImGuiInputTextFlags_EnterReturnsTrue |
		ImGuiInputTextFlags_CallbackCompletion |
		ImGuiInputTextFlags_CallbackHistory;
		if (ImGui::Button("Clear")) { ClearLog(); }
		ImGui::SameLine();
		/*if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this))
		{
				char* s = InputBuf;
				Strtrim(s);
				if (s[0])
						ExecCommand(s);
				strcpy(s, "");
				reclaim_focus = true;
		}
		*/
		// Auto-focus on window apparition
		ImGui::SetItemDefaultFocus();
		if (reclaim_focus) ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

		//ImGui::End();
	}

	void ExecCommand(const char* command_line)
	{
		AddLog("# %s\n", command_line);

		// Insert into history. First find match and delete it so it can be pushed to the back.
		// This isn't trying to be smart or optimal.
		HistoryPos = -1;
		for (int i = History.Size - 1; i >= 0; i--)
			if (Stricmp(History[i], command_line) == 0)
			{
				free(History[i]);
				History.erase(History.begin() + i);
				break;
			}
		History.push_back(Strdup(command_line));

		// Process command
		if (Stricmp(command_line, "CLEAR") == 0)
		{
			ClearLog();
		}
		else if (Stricmp(command_line, "HELP") == 0)
		{
			AddLog("Commands:");
			for (int i = 0; i < Commands.Size; i++) AddLog("- %s", Commands[i]);
		}
		else if (Stricmp(command_line, "HISTORY") == 0)
		{
			int first = History.Size - 10;
			for (int i = first > 0 ? first : 0; i < History.Size; i++) AddLog("%3d: %s\n", i, History[i]);
		}
		else
		{
			AddLog("Unknown command: '%s'\n", command_line);
		}

		// On command input, we scroll to bottom even if AutoScroll==false
		ScrollToBottom = true;
	}

	// In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
	static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
	{
		NeoniteGUI* console = static_cast<NeoniteGUI*>(data->UserData);
		return console->TextEditCallback(data);
	}

	int TextEditCallback(ImGuiInputTextCallbackData* data)
	{
		//AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
		switch (data->EventFlag)
		{
		case ImGuiInputTextFlags_CallbackCompletion:
		{
			// Example of TEXT COMPLETION

			// Locate beginning of current word
			const char* word_end = data->Buf + data->CursorPos;
			const char* word_start = word_end;
			while (word_start > data->Buf)
			{
				const char c = word_start[-1];
				if (c == ' ' || c == '\t' || c == ',' || c == ';') break;
				word_start--;
			}

			// Build a list of candidates
			ImVector<const char*> candidates;
			for (int i = 0; i < Commands.Size; i++)
				if (Strnicmp(Commands[i], word_start, static_cast<int>(word_end - word_start)) == 0
				)
					candidates.push_back(Commands[i]);

			if (candidates.Size == 0)
			{
				// No match
				AddLog("No match for \"%.*s\"!\n", static_cast<int>(word_end - word_start),
				       word_start);
			}
			else if (candidates.Size == 1)
			{
				// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
				data->DeleteChars(static_cast<int>(word_start - data->Buf),
				                  static_cast<int>(word_end - word_start));
				data->InsertChars(data->CursorPos, candidates[0]);
				data->InsertChars(data->CursorPos, " ");
			}
			else
			{
				// Multiple matches. Complete as much as we can..
				// So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
				int match_len = static_cast<int>(word_end - word_start);
				for (;;)
				{
					int c = 0;
					bool all_candidates_matches = true;
					for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
						if (i == 0) c = toupper(candidates[i][match_len]);
						else if (c == 0 || c != toupper(candidates[i][match_len])) all_candidates_matches = false;
					if (!all_candidates_matches) break;
					match_len++;
				}

				if (match_len > 0)
				{
					data->DeleteChars(static_cast<int>(word_start - data->Buf),
					                  static_cast<int>(word_end - word_start));
					data->InsertChars(data->CursorPos, candidates[0],
					                  candidates[0] + match_len);
				}

				// List matches
				AddLog("Possible matches:\n");
				for (int i = 0; i < candidates.Size; i++) AddLog("- %s\n", candidates[i]);
			}

			break;
		}
		case ImGuiInputTextFlags_CallbackHistory:
		{
			// Example of HISTORY
			const int prev_history_pos = HistoryPos;
			if (data->EventKey == ImGuiKey_UpArrow)
			{
				if (HistoryPos == -1) HistoryPos = History.Size - 1;
				else if (HistoryPos > 0) HistoryPos--;
			}
			else if (data->EventKey == ImGuiKey_DownArrow)
			{
				if (HistoryPos != -1)
					if (++HistoryPos >= History.Size) HistoryPos = -1;
			}

			// A better implementation would preserve the data on the current input line along with cursor position.
			if (prev_history_pos != HistoryPos)
			{
				const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
				data->DeleteChars(0, data->BufTextLen);
				data->InsertChars(0, history_str);
			}
		}
		}
		return 0;
	}
};

inline NeoniteGUI console;
inline NeoniteGUI client;
