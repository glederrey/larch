


import base64, os


# data:image/png;base64
favicon = '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%3D'

eye = 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAMAAACdt4HsAAAABGdBTUEAALGPC/xhBQAAAAFzUkdCAK7OHOkAAABgUExURUxpcQCj2wCPxQCa1QCZ1ACd2QCMwgCDuQCP0wCs3wBvoACOxwCGvACOxwCPxgCY0QCPzwCTzACTywCj1QCa2ACU1gCe2QCl3ACi2wCp3QCj5ACY4ACy6ACs6AC37ACt5OoTzCwAAAAUdFJOUwD7XPrR+0UE/f0WnCxwz6rquoflF5GMKAAAApRJREFUWMPtVtmCoyAQFCGKt0aNkUP+/y+nOQRijk12Z99SM5kM3V0F3ZxJ8sUXX/x35B5/RX7dfoddZum5ApzTrPxIQ8eVaUVwsTkUmFRp6Vzv0LNuFJtQhYeC5thlb0iAP2+I2iKyF9kUafLXEpBl3oybKIrVcMS2Cf0xjVW3R5B4XgtwpCOMfLV06BHqB5WEEVmJFXIZ06eDyJN6UALCDJTq6t1Td0o5cyHUUD9UAFuDxbpDjFlYSVDWMXLh5sEg8qQcRIFcDJJjaSqSJ+6rHKV3FmIojwp5kvViXXYozQ8x+t9yVN69ij47KjRXhZYgkFl+2rVtl1qFLAgsSF2b2/QruQQ+kp2h1K2SANXWptnJKGSRlS8EfA0SXSMstU67xhIZSGzbSxyD5OAUQLy94SNFjIfsViSJiSPqJky2Jk3DP100rub3guRZd5hKdHFAMtWWs7G4qMvltCs4/nUPP9nwzskaS2clveXqFYDPvNmFMzNHJNhPjJiZvo/UYyD/IkD0EAjDU4wTsykEM2Y2BXa6CcSWf6eAmS1iLGCLeIwj+zTcKmDW2mncrZjZaWwPYSTfF0IyMDxP7meGj1tIDBswt5Csc7JhmA3xUqzYvEP7KruUCWOcM0bsUq6glwBWJfFuSxo+Rd7ZbyZC/GaK/RM/nAl6O3Ma/P39du5DD5Tfb2d9oLCZ+oj2eKC0Xp/O7P5AsWlQTulMqf6j+4iPND0+64Gg5uG5qus88Jk6jYlW4VCtoOkcMx/qZye7rlofJDht7bHe6pHt9D59cbeYi8VKGHCuc+Z8bwP95cXirzYeNAJA6I9XW7hc+4MGsPu3Ltf4etcsi5l+cL3HD4zGPjCaDx8Yv/DE+Y1H1hdffPEhfgDKU1gnLBbrLAAAAABJRU5ErkJggg=='

favicon_raw = b'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00 \x00\x00\x00 \x08\x03\x00\x00\x00D\xa4\x8a\xc6\x00\x00\x00\x04gAMA\x00\x00\xb1\x8f\x0b\xfca\x05\x00\x00\x00\x01sRGB\x00\xae\xce\x1c\xe9\x00\x00\x03\x00PLTE\x00\x00\x00\r(\x02\x0e)\x02\x10,\x02\x00\x00\x00\x00\x00\x00\x0c\x1a\x08\x7f\x7f\x7f\x00\x00\x00\x00\x00\x00SSS\x00\x00\x00\x02\x02\x02\x7f\x7f\x7f\x162\n\x02\x03\x02\x177\x0b,_\x16\x00\x00\x00\x00\x04\x00,`\x16\r\x1a\x08\x7f\x7f\x7f\r&\x02\x7f\x7f\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x164\n\x00\x00\x00\x174\n\x00\x00\x00\x178\x0b\x00\x00\x00\x176\n@@@)\\\x14+^\x16,b\x17ccc???\x0b\x15\x05\x0c\x17\x06\x12\x12\x12\x0c#\x02\r&\x02\x03\r\x03\x12#\n\x19\x19\x19""\x17\x00\x00\x00\x12*\t\x1dQ\t\x0e\x0e\x0e\x03\x03\x03\x02\x06\x01\x03\x03\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\'X\x13\x0b\x1b\x05"R\x0e\x0f\x0f\x0f\x162\n\x00\x00\x00\x10/\x03\x00\x00\x00\x07\x10\x04\x00\x00\x00\x00\x00\x00\x0f#\x07"S\x0e\x1a;\x0b\x00\x00\x00\x1a<\x0bU\x7fU(Y\x13\x08\x17\x03\x00\x00\x00\x08\x16\x03+]\x16\x00\x00\x00?\x81#\x00\x00\x00\x00\x00\x00\x08\x11\x03\x00\x00\x00D\x9b I\xa6"C\x99 I\xa7"J\xa9#J\xa8"G\xa1!F\xa0!D\x9a G\xa0!G\xa2!F\x9f E\x9d H\xa3!K\xab#H\xa4!M\xaf$\x01\x02\x00P\xb6%K\xac#B\x97\x1f=\x8b\x1c@\x94\x1cT\xc0\'L\xae$E\x9e X\xc8)I\xa5"J\xaa#L\xad#C\x97\x1fA\x96\x1dS\xbd\'O\xb5%?\x93\x1bK\xaa#L\xac#M\xb0#\x02\x04\x01\x03\x07\x01@\x91\x1eD\x9c\x1e=\x91\x1bD\x9e\x1f\x12*\x08D\x9c V\xc5(A\x94\x1e\x14-\t\x08\x13\x04Z\xcf*\r\x1e\x06A\x98\x1d@\x97\x1cF\x9e!>\x8d\x1cB\x99\x1e=\x8e\x1c:\x89\x1aC\x9b\x1e,f\x15C\x9a\x1f\x15/\tN\xb3$\x02\x05\x01T\xbf\'P\xb7%;\x87\x1bM\xb1$1p\x16R\xb6)3u\x18+c\x14S\xb8)B\x96\x1f$R\x11<\x8a\x1c:\x86\x1b\x162\n\x07\x10\x03W\xc6)?\x90\x1eY\xcb)\\\xd1+A\x95\x1fU\xc3(^\xd8,K\xa6&;\x8b\x1aG\x9d$M\xac\'?\x8f\x1dJ\xa9"O\xb0(8\x82\x1aR\xbc&*^\x13M\xb1%:\x83\x1a2r\x17>\x90\x1c5|\x185z\x19H\xa8!Q\xb8&M\xae$:\x85\x1bK\xac$W\xc7)\x0f#\x07Q\xb3(E\xa1\x1f\x04\x08\x02O\xb4%\x10$\x07R\xbb&\x1cA\rS\xbe\'\'Y\x12\x163\n\x04\x0b\x02\x0b\x1a\x05@\x93\x1d\r\x1d\x06U\xc2(\\\xd2+\'W\x12]\xd5,$T\x11<\x8c\x1a@\x91 A\x95\x1fJ\xa3%L\xa9\'K\xa5&E\x9b!Q\xa9-E\x9d!@\x93\x1fR\xbc\'-g\x15M\xad#6\x7f\x18O\xae\'D\xa0\x1fP\xb1(S\xb7)0m\x16\x19:\x0c-i\x15F\xa3 *`\x13J\xad"R\xb5(\\\xd3+b\xde-Q\xb9&P\xb3%1q\x17/k\x16X\xc7)^\xd5,\x186\x0b\x0f!\x07#Q\x11\x11\'\x08 I\x0f\n\x17\x05}\xb8;O\x00\x00\x00WtRNS\x00\xbf\xbf\xbf#\x7f\xb9\x04\x1d\x1b\x02!\x16\x06\xf1\x81\xf1\xf5\x07\x19\xf5\xb9\x10\xbe\rs}1\xf1i\xf1\'\xf0\x86\xf2\x06\xf5\xf5\xf5\t\x04\xb4\xb9\x0e\xb4\xbfM\x96\n\x16]\xec\xf5\x12\xa0\xc7\x9d.ZT\xf3\xd9\xf9\x10\xd8\xf6\xcd\xd4\xfdn|\xf9\xf1\xefz\xef\x06\xf4\x985\xd9\xf5d\xfdK\x92\x9e\x83\x12o\x03\x00\x00\x03$IDAT8\xcbc`\xa0\x02`\xe7\xc5%\xc3\x05\x96\xe1b0\xd4\xd7\x15gdd\x02\x02f0\x00\xb1\x98\x18\x19\xc5u\xf4\x18\x94A\xf2,&S7\xde[\xbf\xe9\xf9\xd6c\x0b\xe7\xbd\xfa\x08$\x16~|\xb9u\xd3\xdd\xf5w6N\xf2cPg\x90`pz\xb0\xb7\x7f\xd2\xc4=y\xe5\xe5y\x8d\x8dI\xfb\x1b\x9b\xcb\x81\xa0j\xcf\x86\r\xbb\x9a\x82C4\x18x\x85<\xaa\xfa#\xee\xc7\xa7G\xc6\x94\xe6\xa7\x9d\x9e}\xad"\xb6\xa22&\xb68\xa9/!*\xf7f\x98\x10\x03;\x8fo\xc2\xc4\x88\xc2\xc4\xb8\x98\x9a\x98\xb7)E\xe7:O~\xf9\x96\xfb5\xb681qj\xce\xcaK\x9a<\x0c\xec\xc2*S\x1b\n\xcb\xe2\xb2\x13#\xb2\x8e.o\xfb\xde\xb9\xfa\\\xf8\x929kJ##\x12ZoUiq2\xb0s\xaaNn/\xfc\x1a\x7fz\xf1\xa9\xb6\xac\xcb-\x8b/.\x08_\xbazngm\xe2\xf4\xd4\x885yl\x1c@\x05\x82\xcd\xed\xd1\xef\x97\xac:s\xaa\xed\xe4\x95\x8e\xf0\xfa\xf0\x86\xb5\xf9\xf3\xeb\x0b\xa6\xef\xc8\x8d|\xd4\x0bQp\xbc!\xfd|x\xf6\x8d\xee\xae\xac\x15k\xbb\xc3W^\x9fsu\xf7\x89\xedo\xc2g\xc7\xaf\xe9\x01+PkN\xa99\xfb\xeb\xe2\xb4\xbfG\xf2\xd7v\x9d\x0f\x9f\xb5\xeal\xd7\xfc\x0bg\xea\x92\xb3"\x13 &\x88\xbc[\xb2o\xef\xa1\xdf\xe1\x1d\xab\xf2W\\\xef\x08\xff\xfa\xf3\xc0\xba\xd7\xe1u\x0b\x96\xcc_\x1a\xd5#\nR\x10\xf8aq\xf8\x89\xf0\xe5\x17\xc2\xaf]\x9d\xb9nYx\xc5\x8fO\xfd32\x97\xcf\xa8\x0b\xef\x8e\xea\x05+P;^\x16\x91\x9c\x14\x9d\xdb\x19~\xa3kuG\xf8\xc1\xcf\xdb\xea\xc3\xa7d\xc7\xa7G\xa5@M\x10yQV\x1cY\xb0\xf3J\xeb\xff\xf0\x7f@\x13\xdav\x85g\xee^1}\xce\x96\xe2\xb8(\x88#E\x9a\xcb\x92\xd2c3\x0e\xe7\'u\x87\xffi\t_0#s_Q\xe9\xce\xea\xe8\xb8\xe4\x1c\xa87\x1b\x17%\xa5\xc6d\x9f\xcc\xc8/\xb9\x1c\x9e\x19\x9eY\xb7\xad(#\xab \r\xa8`\x114\xa0&\xb7\'\'FVgW\xd6\x9e-=\x1a\x9eY\xbf\xf4\x08H>6\xb5/\xa5\xbd\n\xac@uBCa_jtZvVu\xd1\xdc\xf0\xf0eE5\xb3\njb\x8a\xe3Z#n\x81\x15\x08\xabL\x988\x15\xe8\x8d\xc8\xd8\xd2\x92\x8a)\xe1\xe1\x1d\x07j\x0e\xc6D\xa7\'\xa6D\xe5\xacl\xd2\xe2`\xf0\xe1\xb1\xb9\xd4\x9f\x10\x11\x9f\x1a\x19\x9b\x11=\xf3Ix\xf8\xb4\xc71\x19%\xb1\xe9q\x85Qe\xfd759\x18\xd4\x85\x1c\x1e\x1eZ\xb9\xf9\xd1\xa2\xa8\x84\xf8\x84\xa7;\xc2\xc3[\xaaS\xe2S\x12r\xd6\xac\xdc<;gn(7\x03\x83\xbd\xac]\xde\x84\xe4\xb8\xc8\xd8\x92\xda-`\x13\xb6\xa7\xd5\xa6\x95D\xa7\xc7\xa7\xe4<\x0b\x08\xe2a`P\xb6\xe4\xb7\x95\xe3\x93\x91\x93\x13PP\xf2v\x0f\x0fw\xf1\x14P\x10\x10\x90\x93\xe1\xe336\xe7\xf7\xd7\x00\xa6{\x03ikI)VVV~/EY\xe7\xf0pGWE7V\x10\x90\x924\x92\x16\x82\xe4\x0fS\x0en\x16 \xe0\x96\xe7\xb12\xb3\xe0\x17\x96\x07\xf3X\xb89\xb4\xb9H\xcf\x86\xbcbb\x12\x0cT\x06\x00\xd9\xdcJ.\xbbbp\x06\x00\x00\x00\x00IEND\xaeB`\x82'

_use_local_logo = False


def local_logo(filename='logo.png', dir=None):
	if dir is None:
		try:
			import appdirs
		except ImportError:
			return None
		dir = appdirs.user_data_dir('Larch')
	png_file = os.path.join(dir, filename)
	if os.path.exists(png_file):
		return base64.b64encode(open(png_file, "rb").read())
	return None
