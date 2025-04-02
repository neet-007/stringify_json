local M = {}

-- Get the path to your binary (assuming it's in the same directory as this Lua file)
local function get_binary_path()
	local script_path = debug.getinfo(1, "S").source:sub(2)
	local plugin_dir = vim.fn.fnamemodify(script_path, ":h")
	return plugin_dir .. "/serializer" -- Replace with your binary's name
end

function M.GetVisualSelection()
	-- Save the current register and cursor position
	local saved_reg = vim.fn.getreg('"')
	local saved_regtype = vim.fn.getregtype('"')
	local saved_visual = vim.fn.getpos("'<")
	local saved_cursor = vim.fn.getpos(".")

	-- Yank the visual selection into the " register
	vim.cmd('silent! normal! ""y')

	-- Get the yanked text
	local selection = vim.fn.getreg('"')

	-- Restore the register and cursor position
	vim.fn.setreg('"', saved_reg, saved_regtype)
	vim.fn.setpos("'<", saved_visual)
	vim.fn.setpos(".", saved_cursor)

	return selection
end

function M.ProcessTextWithBinary(text)
	local binary_path = get_binary_path()
	print(binary_path)

	-- Escape special characters in the text for shell safety
	local escaped_text = text:gsub("'", "'\\''")

	-- Execute the binary with the text as argument
	local handle = io.popen(binary_path .. " '" .. escaped_text .. "'", "r")
	if not handle then
		error("Failed to execute binary: " .. binary_path)
	end

	-- Read the output
	local result = handle:read("*a")
	handle:close()

	-- Trim whitespace from the end
	result = result:gsub("%s+$", "")

	return result
end

function M.ProcessVisualSelection()
	local mode = vim.api.nvim_get_mode().mode
	if not (mode == "v" or mode == "V" or mode == "") then
		print("Not in visual mode!")
		return
	end

	local selection = M.GetVisualSelection()
	local processed = M.ProcessTextWithBinary(selection)

	-- Save to paste buffer (register)
	vim.fn.setreg('"', processed)
	vim.fn.setreg("+", processed) -- Also set the system clipboard if available
	vim.fn.setreg("*", processed) -- For X11 primary selection

	print("Processed text saved to paste buffer")
end

-- Set up the keymap
-- vim.api.nvim_set_keymap('v', '<leader>js', ':lua require("stringify_json").ProcessVisualSelection()<CR>', { noremap = true, silent = true })

return M
