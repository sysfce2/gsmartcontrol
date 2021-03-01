/******************************************************************************
License: Zlib
Copyright:
	(C) 2008 - 2021 Alexander Shaduri <ashaduri@gmail.com>
******************************************************************************/
/// \file
/// \author Alexander Shaduri
/// \ingroup libdebug
/// \weakgroup libdebug
/// @{

#ifndef LIBDEBUG_DSTATE_H
#define LIBDEBUG_DSTATE_H

#include <string>
#include <map>
#include <stack>
#include <memory>

#include "dflags.h"
#include "dstream.h"
#include "dexcept.h"

#include "dstate_pub.h"  // public interface part of this header



namespace debug_internal {


	// domain name "default" - template for all new domains.
	// domain name "all" - used for manipulating all domains.


	/// Libdebug global state
	class DebugState {
		public:

			/// A mapping of debug levels to respective streams
			using level_map_t = std::map<debug_level::flag, std::shared_ptr<DebugOutStream>>;

			/// A mapping of domains to debug level maps with streams
			using domain_map_t = std::map<std::string, level_map_t>;


			/// Constructor (statically called), calls setup_default_state().
			DebugState() noexcept
			{
				setup_default_state();
			}


			/// Initialize the "default" template domain, set the default enabled levels / format flags.
			/// Automatically called by constructor.
			void setup_default_state() noexcept;


			/// Get the domain/level mapping.
			domain_map_t& get_domain_map()
			{
				return domain_map;
			}


			/// Get current indentation level.
			int get_indent_level() const
			{
				return indent_level_;
			}

			/// Set current indentation level.
			void set_indent_level(int indent_level)
			{
				indent_level_ = indent_level;
			}

			/// Open a debug_begin() context.
			void push_inside_begin(bool value = true)
			{
				inside_begin_.push(value);
			}

			/// Close a debug_begin() context.
			bool pop_inside_begin()
			{
				if (inside_begin_.empty())
					throw debug_usage_error("DebugState::pop_inside_begin(): Begin / End stack underflow! Mismatched begin()/end()?");
				bool val = inside_begin_.top();
				inside_begin_.pop();
				return val;
			}

			/// Check if we're inside a debug_begin() context.
			bool get_inside_begin() const
			{
				if (inside_begin_.empty())
					return false;
				return inside_begin_.top();
			}


			/// Flush all the stream buffers. This will write prefixes too.
			void force_output()
			{
				for (auto& iter : domain_map) {
					for (auto& iter2 : iter.second)
						iter2.second->force_output();
				}
			}


		private:

			int indent_level_ = 0;  ///< Current indentation level
			std::stack<bool> inside_begin_;  ///< True if inside debug_begin() / debug_end() block

			domain_map_t domain_map;  ///< Domain / debug level mapping.

	};



	/// Get global libdebug state
	DebugState& get_debug_state();



}  // ns







#endif

/// @}