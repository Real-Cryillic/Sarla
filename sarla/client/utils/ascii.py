from rich.padding import Padding


def print_ascii(console):
    ascii = """\
   ▄▄▄▄▄   ██   █▄▄▄▄ █    ██   ▄█▄    ▄█▄    
  █     ▀▄ █ █  █  ▄▀ █    █ █  █▀ ▀▄  █▀ ▀▄  
▄  ▀▀▀▀▄   █▄▄█ █▀▀▌  █    █▄▄█ █   ▀  █   ▀  
 ▀▄▄▄▄▀    █  █ █  █  ███▄ █  █ █▄  ▄▀ █▄  ▄▀ 
              █   █       ▀   █ ▀███▀  ▀███▀  
             █   ▀           █                
            ▀               ▀       
    """

    output = Padding(ascii, (1, 2), style="#ff5555")
    console.print(output)
