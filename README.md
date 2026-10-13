# opensurgbot

[opensurgbot](https://github.com/lgabp1/opensurgbot) root repository.

## About

opensurgbot aims to be an open-source, low-cost, easy-to-use and easy-to-improve surgical robot platform, compatible with the [SurRoL](https://github.com/med-air/SurRoL) framework.

The first prototype has been successfully built and tested.

## Repositories

| Repository | Description |
|------------|-------------|
| [opensurgbot](https://github.com/lgabp1/opensurgbot) | opensurgbot root repository |
| (no link yet) | Documentation repository  |
| [opensurgbot_SurRoL](https://github.com/lgabp1/opensurgbot_SurRoL/) | Integration of opensurgbot with the SurRoL framework |
| [opensurgbot_pipeline](https://github.com/lgabp1/opensurgbot_pipeline/) | High level python api and microcontroller code |
| [opensurgbot_kinevizu](https://github.com/lgabp1/opensurgbot_kinevizu/) | 3D visualization of the da Vinci Large Needle Drive and kinematics model |
| [opensurgbot_board](https://github.com/lgabp1/opensurgbot_board/) | PCB design files for the opensurgbot custom board |
| [opensurgbot_3Dprinted](https://github.com/suyu-lily/opensurgbot_3Dprinted) | 3D printed parts of the opensurgbot platform |
| [opensurgbot/web](https://github.com/lgabp1/opensurgbot/tree/master/web) | GitHub page of the project |

## Installation and usage

- Please refer to [opensurgbot_pipeline/STM32CubeIDE_projects](https://github.com/lgabp1/opensurgbot_pipeline/tree/main/STM32CubeIDE_projects) (based on [DM-MC02_drivers](https://github.com/lgabp1/DM-MC02_drivers) and [ZDT stm32 driver](github.com/lgabp1/zdt_stm32_driver)) to setup the STM32 microcontroller.
- Please refer to [(no link yet)]() and [(no link yet)]() for the hardware setup. The optional [opensurgbot_board](https://github.com/lgabp1/opensurgbot_board/) can be used to simplify the wiring.

As for the software,
- Please refer to [opensurgbot_pipeline](https://github.com/lgabp1/opensurgbot_pipeline/) to use the high level python API.
- Please refer to [opensurgbot_SurRoL](https://github.com/lgabp1/opensurgbot_SurRoL/) for the integration with the SurRoL framework.