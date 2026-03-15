

package.loaded["base.class"]= nil
package.loaded["byyl.production"]= nil
package.loaded["byyl.production_set"]= nil


local Production_Set = require("byyl.production_set")
p_set = Production_Set()
p_set:Add(0, "S", "NVN")
p_set:Add(1, "N", "s")


local No_Terminal = require("byyl.no_terminal")
local no_terminal = No_Terminal()
print(no_terminal)
