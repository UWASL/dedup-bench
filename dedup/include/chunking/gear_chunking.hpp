#ifndef _Gear_CHUNKING_
#define _Gear_CHUNKING_

#include "chunking_common.hpp"
#include "config.hpp"

class Gear_Chunking : public virtual Chunking_Technique {
    /**
     * @brief Class implementing gear's based chunking
     *
     */

   private:
    uint64_t min_block_size;
    uint64_t max_block_size;
    uint64_t avg_block_size;
    uint64_t mask;



    const uint64_t GEAR_TABLE[256] = {
        0xd1a16514dc206650, 0x4ddab180952e6a74, 0x7ed1d26a9f4a2d9b,
        0x2cc94adb288d1aec, 0x4339166e5035ca2e, 0xab9091be05d6f529,
        0xc75ffd54c19b9516, 0x207c8975c69bc35b, 0x08db87e31402eadc,
        0xfe4cd514c456d564, 0xd1420ff8db82c911, 0xade479a619a7636e,
        0x7217bae7640502f8, 0x17301bb4b2c9ff93, 0xe2f67d4293bea7c9,
        0x60c1b64503f1ae5f, 0xd3317230c6a89a4a, 0x54b6d40bb023d9ff,
        0x553f965dd89c9402, 0x322cf2b15d470119, 0xedf13527b6437e84,
        0x76745c0906d407c4, 0xc60da1497539ab11, 0x0ba7776f4792f8e5,
        0x2a7f551d8c5b4d31, 0x0cfead877ab063d5, 0x6e678a7838bb038b,
        0x875ed8d97acaebe7, 0x8aed1a12d4c3fbca, 0xe023e6ab9feeb8df,
        0xfdd21afbe2392ccf, 0x5fef7d88af28f38a, 0xe66d9569dc755d82,
        0x069488f524a68608, 0x4edb62a389057897, 0x970b0ec0227c306f,
        0xe2a50bdde312c3e8, 0x897a3917ad2d95e2, 0x4042ac5f9b83f96e,
        0x0325ab80db98eb7b, 0x842a6d834f0c225e, 0xfeedbeed9dae9a7d,
        0x8a12e02102a0b76c, 0xbfca933b7e85c4c8, 0x4d2d5efc211816d4,
        0x46b9a972083da108, 0xe0739db4c30eea4e, 0xbe9572ba55ba8a79,
        0x192340397c988b6f, 0xefc5bc5fa4408630, 0xfb5bc99628d59ad4,
        0xe53ec3ad106555d0, 0xacd2e5528d1c8edb, 0xded83bd80e3f6a80,
        0xd4f999f0069cd46d, 0xd4583576d6033deb, 0xdc6db7c69e83e859,
        0x59a1c8d2f3490df6, 0x4c8a746a098326f4, 0xfad28df33893006b,
        0x904406e32ab8dc66, 0x8ccb9c51392b512d, 0xeadb1fb4b22fddad,
        0x9240f9574c68d7f5, 0xc810baaa9aafe71b, 0x56fb3e2ca18ddd82,
        0x4131043a56f2ab16, 0x8992641f86c4e1c7, 0xfb37ec17d4baf904,
        0x63130ee438ae0f10, 0xf508d541c77c4c75, 0x68c415cfdfada5a7,
        0x07435b1908702546, 0x648800f682df4c01, 0x61ffa59e602118df,
        0xd787335b4289b86a, 0x9e62fb0ae8275291, 0x232e312192042bcb,
        0xa383b19bb4d06311, 0xdf5533274a5e899f, 0xfc3ecc60b3379191,
        0x014a552136e40013, 0x03cac0e8af12ef81, 0x3b682bf35abc651a,
        0x153a15d84143c296, 0x2b75664875bc5c44, 0xc1ff08df175bdba0,
        0xc28f94198b8c5bc5, 0x33764ca1509cee8a, 0x361cca0a9f8cb012,
        0x8ac1670c9498daa4, 0x2b63bccce71835d0, 0xc44743d726efe9f9,
        0x640e024d2e15ccc1, 0x29d4c518c0030776, 0xe139450316ac7e78,
        0xe395c05c9c34258d, 0x9d17e41d97809999, 0x88c6cbdd1a3c6ce9,
        0x962aa19bd5b0cb1e, 0x5e57e8667fde63e8, 0x6b9eafafa32707ed,
        0x1b20adeb05623a92, 0xf1a640886be62cf3, 0x05f655d4ebc1e835,
        0x177e24ea167a3da7, 0x0da18f53577cb417, 0x074828ada929b1ec,
        0x7d41698b0752fa52, 0xb71d01536265804d, 0x3f4f7448de07abf9,
        0x2dd1488ddc7c6e30, 0x2fe3573da7b77cc9, 0x55b9f117d610c05b,
        0x4db621a35cbf59ee, 0x96a2af56523d1d2f, 0xf4a744db21c8eedb,
        0xfaf92425521e6566, 0x5e1c09452e85986d, 0x9ee7e422fd0042d1,
        0x3e98973ebf9498d2, 0x84b60bd37abb6f17, 0x6e24b64e2acb1d81,
        0x486aa3cbbf85ac64, 0x71f2d26b9b0c9bab, 0x930f6a821e0ee6f8,
        0x0140107ee1eb554a, 0x55e01d909bb46204, 0x06125038ff7567a8,
        0x00e4eccdff895174, 0x39577406caec0504, 0x5a69e9aafe9a8e22,
        0xb30e393d5b9be940, 0x26cc2166e880cd7c, 0xe9b7fed59189881d,
        0x81a9a42005b1d5bc, 0xb2b82891717da7b2, 0x29cf375fe33643a3,
        0xd855452047893512, 0x2aefd7a703a749e6, 0xe135c6dbb52f9b43,
        0x22a8943e8e553598, 0x3ec40771a98728c6, 0x0299d18b770d2153,
        0x17d48b6bfe72a3d5, 0x24f07ecde7c3012d, 0x68323e439a196a35,
        0x7f5b0c6639f764e3, 0xa82d0805ac3b581d, 0x2ab139ec02b9f48e,
        0xa7d49bc197741231, 0x773e5f61bfac6c5f, 0xfa594af4be83d15e,
        0x6ecc4b7dbffa739f, 0x1865c8e2f94f345b, 0x9a3e90d88465170f,
        0x58c7b269955e9399, 0x4ff674d2780706a2, 0x455cbc9d3a26d266,
        0x6db8efbc17a17de7, 0xab1815bb95d92f95, 0x27f722912951f49e,
        0x5012c0a02442d9a5, 0x56583c2ad17a9d70, 0xd98debe4c52efe4e,
        0x99b7d5deddab7ee0, 0x85053d6e7721ee0e, 0x405af699372d70fc,
        0xcfac3959e13dcde3, 0xdb81bcf35a21461f, 0x011f1cce71738957,
        0xfcb2987cae0be3af, 0xf6935b228f0dc9dc, 0xa1e8f1cdac3c419a,
        0x3d41d54374c0a785, 0x696a68d1427c79e4, 0x77612ac115f0dabf,
        0xb17f39f98aca63ee, 0x1683a39ad034d6f3, 0x9e08ada4f55ac364,
        0x93bdc0f58a4ca761, 0x9510b346802b60c1, 0xdadf342305fd2a34,
        0x671800567cb99f7f, 0xb5c5363da88ab452, 0x0083bb39fb8f8024,
        0xb0b3dabd9f1841c0, 0x29747a3659b0aa27, 0x81906dfcfce396d6,
        0x06ee8cff0c633218, 0xe661c3160d389b8e, 0x577432f794816eeb,
        0x8bd7ffa707da0c0c, 0x2a3872e640f7cb2d, 0x815876c1a4de4351,
        0x6a8c240e8fec210b, 0xef2db3668fd73aec, 0x6d21b323856d8713,
        0xfd33d36416600d5f, 0xd513562868570a05, 0x9de4dd38c1ff025c,
        0xff835dc7599e827e, 0x1a17135a3c11df40, 0x1f4a2b5645503c81,
        0x4c0ff8ce6e28b27f, 0x2c0b30b9c350e9ee, 0xeb8eb60de4e6a93b,
        0x916c0f72057967f1, 0x15ae40496653de12, 0xac7a8a7023e8351e,
        0xa46dd5ec367648e4, 0xc486e30392e894bb, 0x2eb6490d594f48c1,
        0xc352ae1e0b627c8f, 0x97d9cd76b487b833, 0x77abb6c8f1659e98,
        0x17a8d48abb530f9a, 0x6c05900b7a4eca7a, 0xaa7a0e9a3dec62f2,
        0x99f6010404b9a3a2, 0x6db16d9cc11fa590, 0x88eb992abfca940a,
        0x6575749821bbcb5c, 0x6c3726b4adacf370, 0x52933b292d31756c,
        0x27c495baf3679571, 0x6ff0eb582e25528f, 0x3d9341714c0f84d2,
        0x8dcfdf60a27d5360, 0xada55b1f10a215b5, 0x4d93651bbcfc82b7,
        0x9494ec69a9020b07, 0x0de06b986239c3f7, 0xbb1f3424c66b5430,
        0x90f0b1362593f6a3, 0xefb0a868cbdde00d, 0x60e76f663824826c,
        0x78e635fb534f0f19, 0x291f832371aef5ec, 0x3c6c84ae20e63084,
        0x9bfcc6f845ae22d5, 0xe3e27c8d935b3747, 0x9a7efd84b0bf91de,
        0xee82fdbe4137cfe4, 0x9a5bdf951ed2bb7c, 0x542ca5a22dd37bcf,
        0x43d201d5c115c883, 0x88c3656920e8a0d4, 0xd5de586a7c7f9d38,
        0xbbdcccb1a9ffe942, 0xc2274eaf06595785, 0xcea374f00a2e840c,
        0x39cdeba6743f033a, 0x409adf6a28300f82, 0x4bc5012751032c2f,
        0x08c956ac24d7ba44,
    };

    /**
     * @brief calculate the next gear hash
     * @param h the current hash value
     * @param ch the next byte to add
     * @return the new hash value
     */
    uint64_t ghash(uint64_t h, unsigned char ch);

    /**
     * @brief finds the next cut point in an array of bytes
     * @param buff: the buff to find the cutpoint in.
     * @param size: the size of the buffer
     * @return: cutpoint position in the buffer 
     */
    uint64_t find_cutpoint(char* buff, uint64_t size) override;


   public:
    /**
     * @brief Default constructor. defines all parameters to defualt values
     * @return: void
     */
    Gear_Chunking();

    /**
     * @brief Defines all parameters based on values from the config file
     * @return: void
     */
    Gear_Chunking(const Config& config);

    /**
     * @brief chunk a file using gear hasing
     * @param data Data stream to chunk.
     * @return A vector of File_Chuncks
     */
    std::vector<File_Chunk> chunk_file(std::string file_path);
};

#endif