from math import log2, ceil

def get_address_info(blocksize: int, tlb_size: int, num: int) -> str:
  """Calculate the virtual page number, tag, index and offset of a given address

  Arguments:
      blocksize {int} -- Size of a single memory block (page)
      tlb_size {int} -- Size of the TLB in entries
      num {int} -- The address to calculate values for (in decimal)

  Returns:
      str -- Formatted string with the virtual page number, tag, index and offset
  """
  numOffsetBits = ceil(log2(blocksize))
  numIndexBits = ceil(log2(tlb_size))

  # Check if num is devisable by 4 -> alligned
  if num % 4 != 0:
    return "Not alligned" 

  offset = num & (2**numOffsetBits - 1)
  index = (num >> numOffsetBits) & (2**numIndexBits - 1)
  tag = num >> (numOffsetBits + numIndexBits)
  virtual_page_number = num >> numOffsetBits

  return f"vp: {virtual_page_number}, tag: {tag}, index: {index}, offset: {offset}"
