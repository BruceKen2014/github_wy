
--编译原理
--采用跳转表方案实现浮点数dfa
--参考视频：https://www.bilibili.com/video/BV17W41187gL?spm_id_from=333.788.videopod.episodes&vd_source=220188dae404f2d7dbd030327e0acc36&p=26
function check_is_digit(char)
    if char >= "0" and char <= "9" then
        return true
    end
    return false
end

function check_number_dfa(input_str)
    local index = 1
    local len = string.len(input_str)
    local char

    local function get_char()
        if index <= len then
            local char = input_str:sub(index,index)
            print(index , " ", char)
            index = index + 1
            return char 
        end
        return nil
    end
    ---[[
    ::g1::
        char = get_char()
        if char == "+" or char == "-" then
            goto g2
        end
        if check_is_digit(char) then
            goto g3
        end
        if char == "." then
            goto g5
        end
        print("g1 got wrong char ", char)
        goto error
    ::g2::
        char = get_char()
        if check_is_digit(char) then
            goto g3
        end
        if char == "." then
            goto g5
        end
        print("g2 got wrong char ", char)
        goto error
    ::g3::
        char = get_char()
        if check_is_digit(char) then
            goto g3
        end
        if char == "." then
            goto g4
        end
        print("g3 got wrong char ", char)
        goto error
    ::g4::
        char = get_char()
        if not char then
            goto success_float
        end
        if check_is_digit(char) then
            goto g4
        end

        print("g4 got wrong char ", char)
        goto error
    ::g5::
        char = get_char()
        if check_is_digit(char) then
            goto g6
        end
        print("g5 got wrong char ", char)
        goto error
    ::g6::
        char = get_char()
        if not char then
            goto success_float
        end
        if check_is_digit(char) then
            goto g6
        end
        print("g6 got wrong char ", char)
        goto error
        --]] 
    ::error::
    print("error")
    do return end
    ::success_float::
    print("success_float")
    do return end
end

local input_str = "123.36f"

check_number_dfa(input_str)
