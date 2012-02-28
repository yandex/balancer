function range(i, j)
   local function rangeGenerator()
      while i < j do
         coroutine.yield(i)
         i = i + 1
      end
   end

   return coroutine.wrap(rangeGenerator)
end
