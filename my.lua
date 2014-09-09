
function wrex_lua(user_data1, user_data2)
    new_data = test_c_api(user_data1)
    
    print(new_data.basic.user)
    print(new_data.basic.level)
    print(new_data.basic.gold)
    
    print(new_data.process[0])
    print(new_data.process[1])
    print(new_data.process[2])
    print(new_data.process[3])
    print(new_data.process[4])
    
    return new_data, user_data2
end

function test_lua()
    print("yes")
end



