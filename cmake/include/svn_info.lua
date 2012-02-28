INDENT = "    "

function escape(str)
    str = string.gsub(str, "\\", "\\\\")
    str = string.gsub(str, "\"", "\\\"")
    str = string.gsub(str, "\r", "\\r")
    str = string.gsub(str, "\n", "\\n")
    return str
end

function file_exists(filename)
    local f = io.open(filename, "r")
    if (f == nil) then
        return 0
    else
        io.close(f)
        return 1
    end
end

function preadline(command)
    local h = io.popen(command, 'r')
    if (h ~= nil) then
        local data = h:read()
        io.close(h)
        return data
    end
    return nil
end

function get_svn_revision(fname)
    local svn = io.popen('svn info ' .. fname)
    local url, rev, author

    if (svn == nil) then return "-1" end
        
    for line in svn:lines() do
        if (string.find(line, '^Revision: ') ~= nil) then
            rev = line
        end
    end

    svn:close()

    if rev == nil then return "-1" end

    return string.sub(rev, 11, -1)
end

function get_svn_data(fname)
    local svn = io.popen('svn info ' .. fname)
    local url, rev, author

    if (svn == nil) then return nil end
        
    for line in svn:lines() do
        if (string.find(line, '^URL: ') ~= nil) then
            url = line
        end
        if (string.find(line, '^Revision: ') ~= nil) then
            rev = line
        end
        if (string.find(line, '^Last Changed Author: ') ~= nil) then
            author = line
        end
    end

    svn:close()

    if (url == nil or rev == nil or author == nil) then return nil end

    return "Svn info:\\n" ..
        INDENT .. escape(url) .. "\\n" ..
        INDENT .. escape(rev) .. "\\n" ..
        INDENT .. escape(author) .. "\\n"
end

function get_git_data(src_dir)
    if (not file_exists(src_dir..".git/config")) then
        return nil
    end

    if (not file_exists("/bin/sh")) then  -- don't care about windows
        return nil
    end

    local commit = preadline('git --git-dir "'..src_dir..'/.git" rev-parse HEAD')
    if (commit == nil or string.len(commit) ~= 40) then
        return nil
    end

    local out_data = "git info:\\n"
    out_data = out_data..INDENT.."Commit: "..commit.."\\n"

    local author = preadline('git --git-dir "'..src_dir..'/.git" log -1 "--format=format:%an <%ae>" '..commit)
    if (author ~= nil) then
        out_data = out_data..INDENT.."Author: "..escape(author).."\\n"
    end

    local subj = preadline('git --git-dir "'..src_dir..'/.git" log -1 "--format=format:%s" '..commit)
    if (subj ~= nil) then
        out_data = out_data..INDENT.."Summary: "..escape(subj).."\\n"
    end

    local gitsvnid = preadline('git --git-dir "'..src_dir..'/.git" log -1 --grep="^git-svn-id: " | grep "^    git-svn-id: .*@.*"')
    if (gitsvnid ~= nil) then
        gitsvnid = string.sub(gitsvnid, 17, -1)
        local at = string.find(gitsvnid, '@')
        local sp = string.find(gitsvnid, ' ', at)
        out_data = out_data.."\\ngit-svn info:\\n"
        out_data = out_data..INDENT.."URL: "..string.sub(gitsvnid, 0, at-1).."\\n"
        out_data = out_data..INDENT.."Revision: "..string.sub(gitsvnid, at+1, sp-1).."\\n"
    end

    return out_data
end

function get_scm_data(src_dir)
    local result = ""

    result = get_svn_data(src_dir)
    if (result ~= nil) then
        return result
    end

    result = get_git_data(src_dir)
    if (result ~= nil) then
        return result
    end

    return "Svn info:\\n"..INDENT.."no svn info\\n"
end

function get_local_cmake_info(src_dir, build_dir)
    -- List of directories checked by INIT_CONFIG macro
    local dirs = { src_dir.."/..", src_dir, build_dir.."/..", build_dir }

    local local_cmake = ""
    for i=1,4 do
        local f = io.open(dirs[i].."/local.cmake", "r")
        if (f ~= nil) then
            for line in f:lines() do
                local_cmake = local_cmake..INDENT..INDENT..escape(line).."\\n"
            end
            io.close(f)
        end
    end

    if local_cmake == "" then
        return ""
    else
        return INDENT.."local.cmake:\\n"..local_cmake
    end
end

function get_other_data(src_dir, build_dir)
    local out_data = "Other info:\\n"

    local user = os.getenv("USER")
    if (user == nil) then
        user = "Unknown user"
    end

    local hostname = os.hostname()
    if (hostname == nil) then
        hostname = "No host information"
    end

 
    local build_date = os.date()

    out_data = out_data..INDENT.."Build by: "..user.."\\n"
    out_data = out_data..INDENT.."Top src dir: "..src_dir.."\\n"
    if (build_dir ~= nil) then
        out_data = out_data..INDENT.."Top build dir: "..build_dir.."\\n"
    end
    out_data = out_data..INDENT.."Build date: "..build_date.."\\n"
    out_data = out_data..INDENT.."Hostname: "..hostname.."\\n"

    out_data = out_data..get_local_cmake_info(src_dir, build_dir)

    return out_data
end

function get_compiler_version(compiler)
    local cc = io.popen(compiler .. ' --version')

    if (cc == nil) then return nil end
    local ver = ""
    for line in cc:lines() do
        ver = ver..INDENT..INDENT..escape(line).."\\n"
    end
    cc:close()

    return ver
end

function get_build_info(compiler, fname)
    local out_data = "Build info:\\n"
    out_data = out_data..INDENT.."Compiler: "..compiler.."\\n"

    out_data = out_data..INDENT.."Compiler version: \\n"..get_compiler_version(compiler)

    local flags_info = INDENT.."Compile flags:"..fname.." no flags info\\n"
    local f = io.open(fname, "r")
    if (f ~= nil) then
        for line in f:lines() do
            local a, b = line:find("CXX_FLAGS")
            if a ~= nil and a == 1 then
                flags_info = INDENT.."Compile flags: "..escape(line:sub(13))
                break
            end
        end
        io.close(f)
    end

    return out_data..flags_info
end

print([[#pragma once

#define PROGRAM_VERSION "]]..get_scm_data(arg[1]).."\\n"..get_other_data(arg[1], arg[4]).."\\n"..get_build_info(arg[2], arg[3])..[["
#define ARCADIA_SOURCE_PATH "]]..arg[1]..[["
#define ARCADIA_SOURCE_REVISION "]]..get_svn_revision(arg[1])..[["

]])

