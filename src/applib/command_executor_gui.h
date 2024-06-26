/******************************************************************************
License: GNU General Public License v3.0 only
Copyright:
	(C) 2008 - 2021 Alexander Shaduri <ashaduri@gmail.com>
******************************************************************************/
/// \file
/// \author Alexander Shaduri
/// \ingroup applib
/// \weakgroup applib
/// @{

#ifndef COMMAND_EXECUTOR_GUI_H
#define COMMAND_EXECUTOR_GUI_H

#include <glibmm.h>
#include <gtkmm.h>
#include <memory>

#include "command_executor.h"



/// Same as CommandExecutor, but with GTK UI support.
/// This one is noncopyable, because we can't copy the dialogs, etc.
class CommandExecutorGui : public CommandExecutor {
	public:

		/// Constructor
		CommandExecutorGui(std::string cmd, std::vector<std::string> cmdargs)
				: CommandExecutor(std::move(cmd), std::move(cmdargs))
		{
			signal_execute_tick().connect(sigc::mem_fun(*this, &CommandExecutorGui::execute_tick_func));
		}


		/// Constructor
		CommandExecutorGui()
		{
			signal_execute_tick().connect(sigc::mem_fun(*this, &CommandExecutorGui::execute_tick_func));
		}



		// Reimplemented from CommandExecutor
		bool execute() override;


		/// UI callbacks may use this to abort execution.
		void set_should_abort()
		{
			should_abort_ = true;
		}


		/// Create a "running" dialog or return already existing one.
		/// The dialog will be auto-created and displayed on execute().
		/// You need the function only if you intend to modify it before execute().
		Gtk::MessageDialog* create_running_dialog(Gtk::Window* parent = nullptr, const Glib::ustring& msg = Glib::ustring());


		/// Return the "running" dialog
		[[nodiscard]] Gtk::MessageDialog* get_running_dialog()
		{
			return running_dialog_.get();
		}


		/// Show or hide the "running" dialog.
		/// This actually shows the dialog only after some time has passed,
		/// to avoid very quick show/hide in case the command exits very quickly.
		void show_hide_dialog(bool show);


		/// This function is called from ticker function in running mode
		/// to show the dialog when requested time elapses.
		void update_dialog_show_timer();


		/// Switch the dialog to "aborting..." mode
		void set_running_dialog_abort_mode(bool aborting);


	private:

		/// Dialog response callback.
		void on_running_dialog_response(int response_id)
		{
			// Try to abort if Cancel was clicked
			if (response_id == Gtk::RESPONSE_CANCEL)
				set_should_abort();
		}


		/// This function is attached to CommandExecutor::signal_execute_tick().
		bool execute_tick_func(TickStatus status);


		bool execution_running_ = false;  ///< If true, the execution is still in progress
		bool should_abort_ = false;  ///< GUI callbacks may set this to abort the execution

		std::unique_ptr<Gtk::MessageDialog> running_dialog_;  ///< "Running" dialog
		bool running_dialog_shown_ = false;  ///< If true, the "running" dialog is visible
		bool running_dialog_abort_mode_ = false;  ///< If true, the "running" dialog is in "aborting..." mode.
		Glib::Timer running_dialog_timer_;  ///< "Running" dialog show timer.

};







#endif

/// @}
